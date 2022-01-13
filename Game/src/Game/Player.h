#pragma once

#include <Game/World.h>
#include <Maths/Types.h>

#include <glm/glm.hpp>

extern const glm::vec3 s_PlayerEyeOffset;

struct Player
{
	glm::vec3 Position, OldPosition;
	glm::vec3 Velocity;
	bool EnableFlight, EnableNoclip;

	float Pitch, Yaw;
	float BaseSpeed, Speed, Sensitivity;
	glm::vec3 Direction, Right, Up;
	glm::mat4 View;

	bool HasChangedBlock;
	bool HasChangedChunk;
};

void PlayerCreate(Player *player, float speed, float sensitivity);

void PlayerUpdate(Player *player, World *world);
void PlayerGetInput(Player *player);
void PlayerUpdatePhysics(Player *player, World *world);
void PlayerCalculateView(Player *player);