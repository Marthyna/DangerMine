#include "collision.h"

Collision::Collision()
{
}

bool Collision::checkForGroundCollision(Camera camera, glm::vec3 ground_pos)
{
    if (camera.center_point[1] - 1.2 <= ground_pos[1])
    {
        return true;
    }

    return false;
}