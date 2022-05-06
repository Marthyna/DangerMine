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

class Collision
{
public:
    Collision();
    bool checkForGroundCollision(Camera camera, glm::vec3 ground_pos);
    void checkForBulletsCollision(std::vector<Bullet> &bullets, std::array<std::array<float, 3>, 6> plane_positions);

    glm::vec3 ground_pos;
    Camera camera();
};

#endif
