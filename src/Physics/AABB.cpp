#include <Physics/AABB.h>

#include <algorithm>

Collision RayVsAABB(const glm::vec3 Start, const glm::vec3 Direction, const AABB &target)
{
	Collision collision;
	collision.Collided = false;

	glm::vec3 invdir = {
		1.0f / Direction.x,
		1.0f / Direction.y,
		1.0f / Direction.z
	};

	glm::vec3 tmin = {
		(target.Position.x - Start.x) * invdir.x,
		(target.Position.y - Start.y) * invdir.y,
		(target.Position.z - Start.z) * invdir.z
	};

	glm::vec3 tmax = {
		(target.Position.x + target.Size.x - Start.x) * invdir.x,
		(target.Position.y + target.Size.y - Start.y) * invdir.y,
		(target.Position.z + target.Size.z - Start.z) * invdir.z,
	};

	if ((std::isnan(tmax.x) || std::isnan(tmax.y) || std::isnan(tmax.z))) return collision;
	if ((std::isnan(tmin.x) || std::isnan(tmin.y) || std::isnan(tmin.z))) return collision;

	if (tmin.x > tmax.x) std::swap(tmin.x, tmax.x);
	if (tmin.y > tmax.y) std::swap(tmin.y, tmax.y);
	if (tmin.z > tmax.z) std::swap(tmin.z, tmax.z);

	if ((tmin.x > tmax.y) || (tmin.y > tmax.x))
		return collision;

	float ftmin = tmin.x;
	float ftmax = tmax.x;

	if (tmin.y > ftmin)
		ftmin = tmin.y;

	if (tmax.y < ftmax)
		ftmax = tmax.y;

	if ((ftmin > tmax.z) || (tmin.z > ftmax))
		return collision;

	if (tmin.z > ftmin)
		ftmin = tmin.z;

	if (tmax.z < ftmax)
		ftmax = tmax.z;

	collision.TimeToHit = ftmin;

	if (collision.TimeToHit >= 0.0f)
		collision.Collided = true;

	// Calculate contact point
	collision.Point = Start + collision.TimeToHit * Direction;

	// Calculate normal
	if (tmin.x > tmin.y && tmin.x > tmin.z)
	{
		if (invdir.x < 0)
			collision.Normal = glm::vec3(1.0f, 0.0f, 0.0f);
		else
			collision.Normal = glm::vec3(-1.0f, 0.0f, 0.0f);
	}
	else if (tmin.y > tmin.x && tmin.y > tmin.z)
	{
		if (invdir.y < 0)
			collision.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
		else
			collision.Normal = glm::vec3(0.0f, -1.0f, 0.0f);
	}
	else if (tmin.z > tmin.x && tmin.z > tmin.y)
	{
		if (invdir.z < 0)
			collision.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
		else
			collision.Normal = glm::vec3(0.0f, 0.0f, -1.0f);
	}

	collision.Tmin = tmin;

	return collision;
}