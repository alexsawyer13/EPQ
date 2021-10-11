#pragma once

#include <Game/World.h>

#include <glm/glm.hpp>

struct Player
{
	glm::vec3 Position;
	glm::vec3 Velocity;
	bool EnableFlight, EnableNoclip;

	float Pitch, Yaw;
	float Speed, Sensitivity;
	glm::vec3 Direction, Right, Up;
	glm::mat4 View;
};

void PlayerCreate(Player *player, float speed, float sensitivity);

void PlayerUpdate(Player *player, World *world);
void PlayerGetInput(Player *player);
void PlayerUpdatePhysics(Player *player, World *world);
void PlayerCalculateView(Player *player);