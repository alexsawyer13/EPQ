#pragma once

#include <glm/glm.hpp>

struct AABB
{
	glm::vec3 Position; // Bottom left back;
	glm::vec3 Size;
};

struct Collision
{
	bool Collided;
	glm::vec3 Point;
	glm::vec3 Normal;
	float TimeToHit;
	glm::vec3 Tmin;
};

enum class Collider
{
	None,
	Block,
};

Collision RayVsAABB(const glm::vec3 Start, const glm::vec3 Direction, const AABB &target);