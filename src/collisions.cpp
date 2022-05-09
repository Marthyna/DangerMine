#include "collisions.h"
#ifdef __APPLE__
#include <array>
#endif

Collision::Collision() {}

bool Collision::checkForGroundCollision(Camera camera, glm::vec3 ground_pos)
{
    return (camera.center_point.y - 0.5 >= ground_pos.y); // Teste plano-cubo
};

void Collision::checkForBulletScenaryCollision(std::vector<Bullet> &bullets, std::array<std::array<float, 3>, 6> plane_positions)
{
    if (bullets.size() == 0)
        return;

    for (int i = 0; i < bullets.size(); i++)
        for (int j = 0; j < plane_positions.size(); j++)
        {
            glm::vec4 point = glm::vec4(bullets[i].position[0] - plane_positions[j][0], bullets[i].position[1] - plane_positions[j][1], bullets[i].position[2] - plane_positions[j][2], 0.0f);

            float distance = dotproduct(point, glm::vec4(plane_positions[j][0], plane_positions[j][1], plane_positions[j][2], 0.0f) / norm(glm::vec4(plane_positions[j][0], plane_positions[j][1], plane_positions[j][2], 0.0f)));
            if (-distance <= 0.2)
                if (!bullets.empty())
                    bullets.erase(bullets.begin() + i);
        }
};

void Collision::checkForEnemiesCollision(std::vector<Enemy> enemies)
{
    for (int i = 0; i < enemies.size(); i++)
        for (int j = 1; j < enemies.size(); j++)
            if ((enemies[i].bbox_min.x <= enemies[j].bbox_max.x && enemies[i].bbox_max.x >= enemies[j].bbox_min.x) &&
                (enemies[i].bbox_min.y <= enemies[j].bbox_max.y && enemies[i].bbox_max.y >= enemies[j].bbox_min.y) &&
                (enemies[i].bbox_min.z <= enemies[j].bbox_max.z && enemies[i].bbox_max.z >= enemies[j].bbox_min.z))
            {
                fprintf(stderr, "%d %d colidem \n", i, j);
            }
};

bool Collision::checkForBulletEnemyCollision(std::vector<Enemy> &enemies, std::vector<Bullet> &bullets)
{
    for (int i = 0; i < bullets.size(); i++)
        for (int j = 0; j < enemies.size(); j++)
            if ((bullets[i].position.x >= enemies[j].bbox_min.x && bullets[i].position.x <= enemies[j].bbox_max.x) &&
                (bullets[i].position.y >= enemies[j].bbox_min.y && bullets[i].position.y <= enemies[j].bbox_max.y) &&
                (bullets[i].position.z >= enemies[j].bbox_min.z && bullets[i].position.z <= enemies[j].bbox_max.z))
                if (!enemies.empty() && !bullets.empty())
                {
                    enemies.erase(enemies.begin() + j);
                    bullets.erase(bullets.begin() + i);
                    return true;
                }
    return false;
}

bool Collision::checkForRocksCollision(Player player, std::vector<Rock> rocks)
{
    for (int i = 0; i < rocks.size(); i++)
    {
        if ((player.bbox_min.x <= rocks[i].bbox_max.x && player.bbox_max.x >= rocks[i].bbox_min.x) &&
            (player.bbox_min.y <= rocks[i].bbox_max.y && player.bbox_max.y >= rocks[i].bbox_min.y) &&
            (player.bbox_min.z <= rocks[i].bbox_max.z && player.bbox_max.z >= rocks[i].bbox_min.z))
        {
            // fprintf(stderr, "colidiu");
            return true;
        }
    }
    return false;
}

bool Collision::checkForEnemiesPlayerCollision(Camera camera, std::vector<Enemy> &enemies)
{
    for (int i = 0; i < enemies.size(); i++)
    {
        if ((camera.center_point.x + 0.5f <= enemies[i].bbox_max.x && camera.center_point.x >= enemies[i].bbox_min.x) &&
            (camera.center_point.y + 0.5f <= enemies[i].bbox_max.y && camera.center_point.y >= enemies[i].bbox_min.y) &&
            (camera.center_point.z + 0.5f <= enemies[i].bbox_max.z && camera.center_point.z >= enemies[i].bbox_min.z))
        {
            enemies.erase(enemies.begin() + i);
            return true;
        }
    }
    return false;
}