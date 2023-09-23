#pragma once

#include <Game/World.h>
#include <Maths/Types.h>

#include <glm/glm.hpp>

#include <optional>

struct Raycast
{
	bool Hit;

	glm::vec3 Location;
	ivec3 Block;
	glm::vec3 Normal;
};

// Dir must be normalised
Raycast VoxelRayCast(World *world, const glm::vec3 &start, const glm::vec3 &dir);