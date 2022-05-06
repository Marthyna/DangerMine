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

void Collision::checkForBulletsCollision(std::vector<Bullet> &bullets, std::array<std::array<float, 3>, 6> plane_positions)
{
    if (bullets.size() == 0)
    {
        return;
    }
    for (int i = 0; i < bullets.size(); i++)
    {
        for (int j = 0; j < plane_positions.size(); j++)
        {
            glm::vec4 point = glm::vec4(bullets[i].position[0] - plane_positions[j][0], bullets[i].position[1] - plane_positions[j][1], bullets[i].position[2] - plane_positions[j][2], 0.0f);

            float distance = dotproduct(point, glm::vec4(plane_positions[j][0], plane_positions[j][1], plane_positions[j][2], 0.0f) / norm(glm::vec4(plane_positions[j][0], plane_positions[j][1], plane_positions[j][2], 0.0f)));
            if (-distance <= 0.2)
            {
                if (!bullets.empty())
                {
                    fprintf(stderr, "ERASE I = %d", i);
                    bullets.erase(bullets.begin() + i);
                }
            }
        }
    }
}