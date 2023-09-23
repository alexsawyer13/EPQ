#pragma once

#include <Game/Player.h>

#include <glm/glm.hpp>

struct Plane
{
    glm::vec3 Normal;
    float Distance;

    Plane() = default;

    Plane(const glm::vec3 &position, const glm::vec3 &direction);
};

struct Frustum
{
    union
    {
        Plane Top, Bottom, Left, Right, Far, Near;
        Plane planes[6];
    };

};

Frustum CreateFrustumFromPlayer(Player *player, float aspect, float fovY, float zNear, float zFar);
bool FrustumCheckPoint(Frustum frustum, float x, float y, float z);
bool FrustumCheckCuboid(Frustum frustum, float centreX, float centreY, float centreZ, float sizeX, float sizeY, float sizeZ);
bool FrustumCheckCuboid(Frustum frustum, const glm::vec3 &centre, const glm::vec3 &size);