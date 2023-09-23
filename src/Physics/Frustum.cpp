#include <Physics/Frustum.h>

Plane::Plane(const glm::vec3 &position, const glm::vec3 &direction)
{
    Distance = glm::length(position);
    Normal = direction;
}

Frustum CreateFrustumFromPlayer(Player *player, float fovY, float aspect, float zNear, float zFar)
{
    Frustum frustum;
    const float halfVSide = zFar * tanf(fovY * .5f);
    const float halfHSide = halfVSide * aspect;
    const glm::vec3 frontMultFar = zFar * player->Direction;

    frustum.Near = { player->Position + zNear * player->Direction, player->Direction };
    frustum.Far = { player->Position + frontMultFar, -player->Direction };
    frustum.Right = { player->Position ,
                            glm::cross(player->Up,frontMultFar + player->Right * halfHSide) };
    frustum.Left = { player->Position ,
                            glm::cross(frontMultFar - player->Right * halfHSide, player->Up) };
    frustum.Top = { player->Position ,
                            glm::cross(player->Right, frontMultFar - player->Up * halfVSide) };
    frustum.Bottom = { player->Position ,
                            glm::cross(frontMultFar + player->Up * halfVSide, player->Right) };

    return frustum;
}

bool FrustumCheckPoint(Frustum frustum, float x, float y, float z)
{
    // Check if the point is inside all six planes of the view frustum.
    for (int i = 0; i < 6; i++)
    {
        //if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3(x, y, z)) < 0.0f)
        if (glm::dot(frustum.planes[i].Normal, glm::vec3(x, y, z)) < 0.0f)
        {
            return false;
        }
    }

    return true;
}

bool FrustumCheckCuboid(Frustum frustum, float centreX, float centreY, float centreZ, float sizeX, float sizeY, float sizeZ)
{
	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for (int i = 0; i < 6; i++)
	{
		if (glm::dot(frustum.planes[i].Normal, glm::vec3(centreX-sizeX, centreY-sizeY, centreZ-sizeZ)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(frustum.planes[i].Normal, glm::vec3(centreX + sizeX, centreY - sizeY, centreZ - sizeZ)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(frustum.planes[i].Normal, glm::vec3(centreX - sizeX, centreY + sizeY, centreZ - sizeZ)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(frustum.planes[i].Normal, glm::vec3(centreX - sizeX, centreY - sizeY, centreZ + sizeZ)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(frustum.planes[i].Normal, glm::vec3(centreX + sizeX, centreY + sizeY, centreZ - sizeZ)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(frustum.planes[i].Normal, glm::vec3(centreX + sizeX, centreY - sizeY, centreZ + sizeZ)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(frustum.planes[i].Normal, glm::vec3(centreX - sizeX, centreY + sizeY, centreZ + sizeZ)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(frustum.planes[i].Normal, glm::vec3(centreX + sizeX, centreY + sizeY, centreZ + sizeZ)) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool FrustumCheckCuboid(Frustum frustum, const glm::vec3 &centre, const glm::vec3 &size)
{
	return FrustumCheckCuboid(frustum, centre.x, centre.y, centre.z, size.x, size.y, size.z);
}