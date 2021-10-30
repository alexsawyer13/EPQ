#include <Physics/VoxelRayCast.h>
#include <Core/Profiling.h>
#include <Maths/Maths.h>

#include <spdlog/spdlog.h>

#define VOXEL_EPSILON 1.0e-06

Raycast VoxelRayCast(World *world, const glm::vec3 &start, const glm::vec3 &dir)
{
	glm::vec3 stepSize;
	stepSize.x = sqrt(1 + (dir.y / dir.x) * (dir.y / dir.x) + (dir.z / dir.x) * (dir.z / dir.x));
	stepSize.y = sqrt(1 + (dir.x / dir.y) * (dir.x / dir.y) + (dir.z / dir.y) * (dir.z / dir.y));
	stepSize.z = sqrt(1 + (dir.x / dir.z) * (dir.x / dir.z) + (dir.y / dir.z) * (dir.y / dir.z));

	ivec3 startInt = { RoundToLowest(start.x), RoundToLowest(start.y), RoundToLowest(start.z) };
	glm::vec3 rayLength;
	ivec3 step = { 0 };

	if (dir.x < 0)
	{
		step.x = -1;
		rayLength.x = (start.x - startInt.x) * stepSize.x;
	}
	else
	{
		step.x = 1;
		rayLength.x = (1 - (start.x - startInt.x)) * stepSize.x;
	}

	if (dir.y < 0)
	{
		step.y = -1;
		rayLength.y = (start.y - startInt.y) * stepSize.y;
	}
	else
	{
		step.y = 1;
		rayLength.y = (1 - (start.y - startInt.y)) * stepSize.y;
	}

	if (dir.z < 0)
	{
		rayLength.z = (start.z - startInt.z) * stepSize.z;
		step.z = -1;
	}
	else
	{
		step.z = 1;
		rayLength.z = (1 - (start.z - startInt.z)) * stepSize.z;
	}

	bool tileFound = false;
	float distance = 0.0f;
	float maxDistance = 10.0f;
	while (!tileFound && distance < maxDistance)
	{

		if (rayLength.x <= rayLength.y && rayLength.x <= rayLength.z)
		{
			startInt.x += step.x;
			distance = rayLength.x;
			rayLength.x += stepSize.x;
		}
		else if (rayLength.y <= rayLength.x && rayLength.y <= rayLength.z)
		{
			startInt.y += step.y;
			distance = rayLength.y;
			rayLength.y += stepSize.y;
		}
		else if (rayLength.z <= rayLength.x && rayLength.z <= rayLength.y)
		{
			startInt.z += step.z;
			distance = rayLength.z;
			rayLength.z += stepSize.z;
		}

		if (startInt.y >= 0 && startInt.y < CHUNK_HEIGHT)
		{
			tileFound = WorldGetBlock(world, startInt.x, startInt.y, startInt.z).IsBlock;
		}
	}

	Raycast raycast = { 0 };

	if (tileFound)
	{
		raycast.Hit = true;
		raycast.Location = start + dir * distance;
		raycast.Block = startInt;

		raycast.Normal = raycast.Location - glm::vec3(raycast.Block.x, raycast.Block.y, raycast.Block.z);

		if (raycast.Normal.x >= 1.0f - VOXEL_EPSILON)
			raycast.Normal.x = 1.0f;
		else if (raycast.Normal.x <= VOXEL_EPSILON)
			raycast.Normal.x = -1.0f;
		else
			raycast.Normal.x = 0.0f;

		if (raycast.Normal.y >= 1.0f - VOXEL_EPSILON)
			raycast.Normal.y = 1.0f;
		else if (raycast.Normal.y <= VOXEL_EPSILON)
			raycast.Normal.y = -1.0f;
		else
			raycast.Normal.y = 0.0f;

		if (raycast.Normal.z >= 1.0f - VOXEL_EPSILON)
			raycast.Normal.z = 1.0f;
		else if (raycast.Normal.z <= VOXEL_EPSILON)
			raycast.Normal.z = -1.0f;
		else
			raycast.Normal.z = 0.0f;


		if (raycast.Normal.x == 0.0f && raycast.Normal.y == 0.0f && raycast.Normal.z == 0.0f)
			spdlog::critical("VoxelRayCast: all normals are 0.0f");
	}

	return raycast;
}