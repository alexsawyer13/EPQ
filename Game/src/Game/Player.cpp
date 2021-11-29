#include <Game/Player.h>
#include <Core/Core.h>
#include <Maths/Maths.h>
#include <Physics/AABB.h>

#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>

glm::vec3 s_WorldUp(0.0f, 1.0f, 0.0f);
glm::vec3 s_Gravity(0.0f, -1.0f, 0.0f);
constexpr float s_SensitivityModifier = 0.02f;

constexpr float s_EyeLevel = 1.6f;
constexpr float s_PlayerWidth = 0.8f;
constexpr float s_PlayerHeight = 1.8f;
constexpr float s_ForeheadSize = s_PlayerHeight - s_EyeLevel;

constexpr glm::vec3 s_PlayerSize(0.8f, 1.8f, 0.8f);
const glm::vec3 s_PlayerEyeOffset(0.4f, 1.4f, 0.4f);

constexpr int s_PlayerWidthMaxBlocks = 2; // Ceil(s_PlayerWidth + 1)
constexpr int s_PlayerHeightMaxBlocks = 3; // Ceil(s_PlayerHeight + 1)

void PlayerCreate(Player *player, float speed, float sensitivity)
{
	player->Position = glm::vec3(0.0f); // Position of eye level
	player->Velocity = glm::vec3(0.0f);
	player->Pitch = 0.0f;
	player->Yaw = 0.0f;
	player->BaseSpeed = speed;
	player->Speed = speed;
	player->Sensitivity = sensitivity;

	player->EnableFlight = false;
	player->EnableNoclip = false;

	PlayerCalculateView(player);
}

void PlayerUpdate(Player *player, World *world)
{
	if (core.input.Keys[GLFW_KEY_LEFT_CONTROL] == Down)
	{
		core.player.Speed = 10 * core.player.BaseSpeed;
	}
	else
	{
		player->Speed = player->BaseSpeed;
	}

	{
		PROFILE_SCOPE_US("PlayerGetInput");
		PlayerGetInput(player);
	}
	{
		PROFILE_SCOPE_US("PlayerUpdatePhysics");
		PlayerUpdatePhysics(player, world);
	}
	{
		PROFILE_SCOPE_US("PlayerCalculateView");
		PlayerCalculateView(player);
	}
}

void PlayerGetInput(Player *player)
{
	if (core.input.IsMouseCaptured)
	{
		// Movement
		glm::vec3 direction(0.0f);

		// Gets the direction of the player without any vertical component
		glm::vec3 forward = player->Direction;
		forward.y = 0.0f;
		forward = normalize(forward);

		glm::vec3 right = player->Right;
		right.y = 0.0f;
		right = normalize(right);
		
		// Sets the direction the player wants to walk
		if (core.input.Keys[GLFW_KEY_W] & Down)
			direction += forward;
		if (core.input.Keys[GLFW_KEY_S] & Down)
			direction -= forward;
		if (core.input.Keys[GLFW_KEY_D] & Down)
			direction += right;
		if (core.input.Keys[GLFW_KEY_A] & Down)
			direction -= right;

		// Normalises direction if it isn't 0, 0, 0
		if (direction.x == 0.0f && direction.y == 0.0f && direction.z == 0.0f)
			player->Velocity = glm::vec3(0.0f);
		else
			player->Velocity = glm::normalize(direction);

		// Handles flight
		if (player->EnableFlight)
		{
			if (core.input.Keys[GLFW_KEY_SPACE] & Down)
				player->Velocity += s_WorldUp;
			if (core.input.Keys[GLFW_KEY_LEFT_SHIFT] & Down)
				player->Velocity -= s_WorldUp;
		}
		else
		{
			player->Velocity += s_Gravity;
		}

		// Rotation
		player->Yaw += core.input.DeltaX * player->Sensitivity * s_SensitivityModifier;
		player->Pitch += core.input.DeltaY * player->Sensitivity * s_SensitivityModifier;
	}
}

void PlayerUpdatePhysics(Player *player, World *world)
{
	if (player->Velocity.x == 0.0f && player->Velocity.y == 0.0f && player->Velocity.z == 0.0f)
		return;

	// Collisions dont' work at 0,0,0????

	// Calculate the players new position after the update
	glm::vec3 deltaPosition = player->Velocity * (float)core.input.DeltaTime * player->Speed;

	if (!player->EnableNoclip)
	{
		glm::vec3 newPosition = player->Position + deltaPosition;

		// Make a list of all blocks that the player could collide with
		// TODO: Include the blocks on the way to the new point (not really necessary)
		std::vector<AABB> aabbs;

		for (int x = -2; x < s_PlayerWidthMaxBlocks + 2; x++)
		{
			for (int z = -2; z < s_PlayerWidthMaxBlocks + 2; z++)
			{
				for (int y = -2; y < s_PlayerHeightMaxBlocks + 2; y++)
				{
					ivec3 block;
					block.x = RoundToLowest(player->Position.x) + x;
					block.y = RoundToLowest(player->Position.y) + y;
					block.z = RoundToLowest(player->Position.z) + z;
					Collider collider = WorldGetBlock(world, block.x, block.y, block.z).Collider;

					AABB blockBox;
					switch (collider)
					{
					case Collider::Block:
					{
						blockBox.Position = {
							float(block.x),
							float(block.y),
							float(block.z)
						};
						blockBox.Size = glm::vec3(1.0f);
						blockBox.Position -= s_PlayerEyeOffset;
						blockBox.Size += s_PlayerSize;
						aabbs.push_back(blockBox);
						break;
					}
					case Collider::None:
					default:
						break;
					}
				}
			}
		}

		std::vector<std::pair<int, Collision>> collisions; // Int = index in aabbs vector

		// Calculate the collision for each AABB
		for (int i = 0; i < aabbs.size(); i++)
		{
			Collision collision = RayVsAABB(player->Position, deltaPosition, aabbs[i]);
			if (collision.Collided && collision.TimeToHit <= 1.0f)
			{
				collisions.push_back({ i, collision }); // Push collisions that need to be resolved to the vector
			}
		}

		spdlog::debug("There have been {} collisions this frame", collisions.size());

		// Sort collisions from closest TimeToHit to furthest
		std::sort(collisions.begin(), collisions.end(), [](const std::pair<int, Collision> &lhs, const std::pair<int, Collision> &rhs) -> bool
			{
				return lhs.second.TimeToHit < rhs.second.TimeToHit;
			});

		// Resolve collisions closest to furthest
		for (int i = 0; i < collisions.size(); i++)
		{
			Collision &collision = collisions[i].second;
			deltaPosition -= collision.Normal * glm::dot(deltaPosition, collision.Normal) * (1 - collision.TimeToHit);
		}
	}

	player->Position += deltaPosition;
}

void PlayerCalculateView(Player *player)
{
	glm::vec3 position = player->Position + s_PlayerEyeOffset;

	// Clamp pitch
	if (player->Pitch >= 89.9f)
		player->Pitch = 89.9f;
	if (player->Pitch <= -89.9f)
		player->Pitch = -89.9f;

	// Clamp yaw
	if (player->Yaw >= 180.0f)
		player->Yaw -= 360.0f;
	if (player->Yaw <= -180.0f)
		player->Yaw += 360.0f;

	// Recalculate vectors and view matrix
	player->Direction.x = glm::cos(glm::radians(player->Yaw)) * glm::cos(glm::radians(player->Pitch));
	player->Direction.y = glm::sin(glm::radians(player->Pitch));
	player->Direction.z = glm::sin(glm::radians(player->Yaw)) * glm::cos(glm::radians(player->Pitch));
	player->Direction = glm::normalize(player->Direction);

	player->Right = glm::cross(player->Direction, s_WorldUp);
	player->Up = glm::cross(player->Right, s_WorldUp);

	player->View = glm::lookAt(position, position + player->Direction, s_WorldUp);
}