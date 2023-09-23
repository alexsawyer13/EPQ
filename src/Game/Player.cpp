#include <Game/Player.h>
#include <Core/Core.h>
#include <Maths/Maths.h>
#include <Physics/AABB.h>

#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>

glm::vec3 s_WorldUp(0.0f, 1.0f, 0.0f);
constexpr float s_SensitivityModifier = 0.02f;
constexpr float s_Gravity = -0.5f;
constexpr float s_JumpForce = 20.0f;
constexpr float s_TerminalVelocity = -40.0f;
constexpr float s_PlayerHeight = 1.9f;
const glm::vec3 s_PlayerEyeOffset(0.0f, 1.8f, 0.0f);

constexpr float s_PlayerWidth = 0.3f;

glm::vec3 direction(0.0f);

bool PlayerPosX(Player *player);
bool PlayerNegX(Player *player);
bool PlayerPosZ(Player *player);
bool PlayerNegZ(Player *player);
bool PlayerPosY(Player *player, float delta);
bool PlayerNegY(Player *player, float delta);

void PlayerCreate(Player *player, float speed, float sensitivity)
{
	player->Position = glm::vec3(0.0f); // Position of eye level
	player->OldPosition = player->Position;
	player->Velocity = glm::vec3(0.0f);
	player->Pitch = 0.0f;
	player->Yaw = 0.0f;
	player->BaseSpeed = speed;
	player->Speed = speed;
	player->Sensitivity = sensitivity;

	player->EnableFlight = false;
	player->EnableNoclip = false;
	player->Grounded = false;
	player->JumpRequest = false;

	player->HasChangedBlock = false;
	player->HasChangedChunk = false;

	PlayerCalculateView(player);
}

void PlayerUpdate(Player *player, World *world)
{
	player->HasChangedBlock = false;
	player->HasChangedChunk = false;

	if (core.input.Keys[GLFW_KEY_LEFT_CONTROL] == Down)
	{
		core.player.Speed = 1.5f * core.player.BaseSpeed;
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
		// Reset direction
		direction = glm::vec3(0.0f);

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
			direction = glm::vec3(0.0f);
		else
			direction = glm::normalize(direction);

		// Handles flight
		if (player->EnableFlight)
		{
			if (core.input.Keys[GLFW_KEY_SPACE] & Down)
				direction += s_WorldUp;
			if (core.input.Keys[GLFW_KEY_LEFT_SHIFT] & Down)
				direction -= s_WorldUp;
		}
		// Handles jumping
		else
		{
			player->JumpRequest = core.input.Keys[GLFW_KEY_SPACE] & Down;
		}


		// Rotation
		player->Yaw += core.input.DeltaX * player->Sensitivity * s_SensitivityModifier;
		player->Pitch += core.input.DeltaY * player->Sensitivity * s_SensitivityModifier;
	}
}

void PlayerUpdatePhysics(Player *player, World *world)
{
	glm::vec3 deltaPosition(0.0f);

	// Gravity
	
	if (player->Velocity.y > s_TerminalVelocity)
		player->Velocity.y += s_Gravity;

	// Jumping

	if (player->JumpRequest && player->Grounded)
	{
		player->Velocity.y = s_JumpForce;
		player->JumpRequest = false;
	}

	// Apply horizontal movement
	
	player->Velocity.x = direction.x * player->Speed;
	player->Velocity.z = direction.z * player->Speed;
	if (direction.y != 0.0f) player->Velocity.y = direction.y * player->Speed;

	player->Grounded = false;
	if (!player->EnableNoclip)
	{
		if (player->Velocity.x > 0.0f && PlayerPosX(player))
			player->Velocity.x = 0.0f;
		if (player->Velocity.x < 0.0f && PlayerNegX(player))
			player->Velocity.x = 0.0f;
		if (player->Velocity.z > 0.0f && PlayerPosZ(player))
			player->Velocity.z = 0.0f;
		if (player->Velocity.z < 0.0f && PlayerNegZ(player))
			player->Velocity.z = 0.0f;
		if (player->Velocity.y > 0.0f && PlayerPosY(player, player->Velocity.y * (float)core.input.DeltaTime))
			player->Velocity.y = 0.0f;
		if (player->Velocity.y < 0.0f && PlayerNegY(player, player->Velocity.y * (float)core.input.DeltaTime))
			player->Velocity.y = 0.0f;
	}

	deltaPosition = player->Velocity * (float)core.input.DeltaTime;

	// Check if player has changed chunks / block
	
	player->OldPosition = player->Position;
	player->Position += deltaPosition;

	if (RoundToLowest(player->OldPosition.x) != RoundToLowest(player->Position.x) ||
		RoundToLowest(player->OldPosition.y) != RoundToLowest(player->Position.y) ||
		RoundToLowest(player->OldPosition.z) != RoundToLowest(player->Position.z))
	{
		player->HasChangedBlock = true;
	}

	int oldChunkX = RoundToLowest((float)player->OldPosition.x / CHUNK_WIDTH);
	int oldChunkZ = RoundToLowest((float)player->OldPosition.z / CHUNK_WIDTH);
	int chunkX = RoundToLowest((float)player->Position.x / CHUNK_WIDTH);
	int chunkZ = RoundToLowest((float)player->Position.z / CHUNK_WIDTH);

	if (oldChunkX != chunkX || oldChunkZ != chunkZ)
	{
		player->HasChangedChunk = true;
	}
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

bool PlayerPosY(Player *player, float delta)
{
	return (
		WorldGetBlock(&core.world, player->Position.x - s_PlayerWidth, player->Position.y + s_PlayerHeight + delta, player->Position.z - s_PlayerWidth).Collider == Collider::Block
		||
		WorldGetBlock(&core.world, player->Position.x + s_PlayerWidth, player->Position.y + s_PlayerHeight + delta, player->Position.z - s_PlayerWidth).Collider == Collider::Block
		||
		WorldGetBlock(&core.world, player->Position.x - s_PlayerWidth, player->Position.y + s_PlayerHeight + delta, player->Position.z + s_PlayerWidth).Collider == Collider::Block
		||
		WorldGetBlock(&core.world, player->Position.x + s_PlayerWidth, player->Position.y + s_PlayerHeight + delta, player->Position.z + s_PlayerWidth).Collider == Collider::Block
		);
}

bool PlayerNegY(Player *player, float delta)
{
	if (
		WorldGetBlock(&core.world, player->Position.x - s_PlayerWidth, player->Position.y + delta, player->Position.z - s_PlayerWidth).Collider == Collider::Block
		||
		WorldGetBlock(&core.world, player->Position.x + s_PlayerWidth, player->Position.y + delta, player->Position.z - s_PlayerWidth).Collider == Collider::Block
		||
		WorldGetBlock(&core.world, player->Position.x - s_PlayerWidth, player->Position.y + delta, player->Position.z + s_PlayerWidth).Collider == Collider::Block
		||
		WorldGetBlock(&core.world, player->Position.x + s_PlayerWidth, player->Position.y + delta, player->Position.z + s_PlayerWidth).Collider == Collider::Block
		)
	{
		player->Grounded = true;
		return true;
	}
	return false;
}

bool PlayerPosX(Player *player)
{
	return (
		WorldGetBlock(&core.world, player->Position.x + s_PlayerWidth, player->Position.y, player->Position.z).Collider == Collider::Block ||
		WorldGetBlock(&core.world, player->Position.x + s_PlayerWidth, player->Position.y + 1.0f, player->Position.z).Collider == Collider::Block ||
		(WorldGetBlock(&core.world, player->Position.x + s_PlayerWidth, player->Position.y + 2.0f, player->Position.z).Collider == Collider::Block && abs(player->Position.y - RoundToLowest(player->Position.y)) > 2.0f - s_PlayerHeight)
		);
}

bool PlayerNegX(Player *player)
{
	return (
		WorldGetBlock(&core.world, player->Position.x - s_PlayerWidth, player->Position.y, player->Position.z).Collider == Collider::Block ||
		WorldGetBlock(&core.world, player->Position.x - s_PlayerWidth, player->Position.y + 1.0f, player->Position.z).Collider == Collider::Block ||
		(WorldGetBlock(&core.world, player->Position.x - s_PlayerWidth, player->Position.y + 2.0f, player->Position.z).Collider == Collider::Block && abs(player->Position.y - RoundToLowest(player->Position.y)) > 2.0f - s_PlayerHeight)
		);
}

bool PlayerPosZ(Player *player)
{
	return (
		WorldGetBlock(&core.world, player->Position.x, player->Position.y, player->Position.z + s_PlayerWidth).Collider == Collider::Block ||
		WorldGetBlock(&core.world, player->Position.x, player->Position.y + 1.0f, player->Position.z + s_PlayerWidth).Collider == Collider::Block ||
		(WorldGetBlock(&core.world, player->Position.x, player->Position.y + 2.0f, player->Position.z + s_PlayerWidth).Collider == Collider::Block && abs(player->Position.y - RoundToLowest(player->Position.y)) > 2.0f - s_PlayerHeight)
		);
}

bool PlayerNegZ(Player *player)
{
	return (
		WorldGetBlock(&core.world, player->Position.x, player->Position.y, player->Position.z - s_PlayerWidth).Collider == Collider::Block ||
		WorldGetBlock(&core.world, player->Position.x, player->Position.y + 1.0f, player->Position.z - s_PlayerWidth).Collider == Collider::Block ||
		(WorldGetBlock(&core.world, player->Position.x, player->Position.y + 2.0f, player->Position.z - s_PlayerWidth).Collider == Collider::Block && abs(player->Position.y - RoundToLowest(player->Position.y)) > 2.0f - s_PlayerHeight)
		);
}