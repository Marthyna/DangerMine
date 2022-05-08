#include "collision.h"

Collision::Collision() {}

bool Collision::checkForGroundCollision(Camera camera, glm::vec3 ground_pos)
{
    if (camera.center_point[1] - 0.5 <= ground_pos[1])
    {
        return true;
    }

    return false;
};

void Collision::checkForBulletScenaryCollision(std::vector<Bullet> &bullets, std::array<std::array<float, 3>, 6> plane_positions)
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
};

void Collision::checkForEnemiesCollision(std::vector<Enemy> enemies)
{
    for (int i = 0; i < enemies.size(); i++)
    {
        for (int j = 1; j < enemies.size(); j++)
        {
            if (enemies[i].getCenter()[0] - enemies[j].getCenter()[0] < enemies[i].getSize()[0] / 2 + enemies[j].getSize()[0] / 2)
            {
                if (enemies[i].getCenter()[1] - enemies[j].getCenter()[1] < enemies[i].getSize()[1] / 2 + enemies[j].getSize()[1] / 2)
                {
                    if (enemies[i].getCenter()[2] - enemies[j].getCenter()[2] < enemies[i].getSize()[2] / 2 + enemies[j].getSize()[2] / 2)
                    {
                        fprintf(stderr, "%d %d colidem \n", i, j);
                    }
                }
            }
        }
    }
};

bool Collision::checkForBulletEnemyCollision(std::vector<Enemy> &enemies, std::vector<Bullet> &bullets)
{
    for (int i = 0; i < bullets.size(); i++)
    {
        for (int j = 0; j < enemies.size(); j++)
        {
            if ((bullets[i].position.x >= enemies[j].bbox_min.x && bullets[i].position.x <= enemies[j].bbox_max.x) &&
                (bullets[i].position.y >= enemies[j].bbox_min.y && bullets[i].position.y <= enemies[j].bbox_max.y) &&
                (bullets[i].position.z >= enemies[j].bbox_min.z && bullets[i].position.z <= enemies[j].bbox_max.z))
            {
                if (!enemies.empty() && !bullets.empty())
                {
                    enemies.erase(enemies.begin() + j);
                    bullets.erase(bullets.begin() + i);

                    return true;
                }
            }
        }
    }

    return false;
}

bool Collision::checkForRocksCollision(Camera camera, std::vector<Rock> rocks)
{
    for (int i = 0; i < rocks.size(); i++)
    {
        // if ()
    }
}