#ifndef _COLLISION_CLASS_H
#define _COLLISION_CLASS_H

#include "matrices.h"
#include "camera.h"
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <glad/glad.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "enemy.h"
#include "rock.h"

class Collision
{
public:
    Collision();

    glm::vec3 ground_pos;

    bool checkForGroundCollision(Camera camera, glm::vec3 ground_pos);
    void checkForBulletScenaryCollision(std::vector<Bullet> &bullets, std::array<std::array<float, 3>, 6> plane_positions);
    void checkForEnemiesCollision(std::vector<Enemy> enemies);
    bool checkForBulletEnemyCollision(std::vector<Enemy> &enemies, std::vector<Bullet> &bullets);
    bool checkForRocksCollision(Camera camera, std::vector<Rock> rocks);
};

#endif
