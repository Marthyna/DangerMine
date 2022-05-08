#ifndef _PLAYER_CLASS_H
#define _PLAYER_CLASS_H

#include "helpers.h"
#include "matrices.h"
#include <cstdio>
#include <cstdlib>
#include <map>
#include <cmath>
#include <glad/glad.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "sceneobject.h"
#include "camera.h"

class Player
{
public:
    glm::vec3 position;
    glm::vec3 bbox_max;
    glm::vec3 bbox_min;
    float size;
    float rotateX;
    float rotateY;
    float rotateZ;

    Player();
    void draw(std::map<std::string, SceneObject> &g_VirtualScene,
              glm::mat4 &model,
              GLint &bbox_max_uniform,
              GLint &bbox_min_uniform,
              GLint &object_id_uniform,
              GLint &model_uniform,
              int identifier,
              glm::mat4 inverse,
              Camera camera);
};

#endif