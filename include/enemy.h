#ifndef _ENEMY_CLASS_H
#define _ENEMY_CLASS_H

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
#include "bezier.h"
class Enemy
{
public:
    glm::vec3 position;
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 p3;
    glm::vec3 bbox_max;
    glm::vec3 bbox_min;
    float t;

    Enemy();

    void draw(Camera camera, std::map<std::string, SceneObject> &g_VirtualScene, glm::mat4 &model, GLint &bbox_max_uniform, GLint &bbox_min_uniform, GLint &object_id_uniform, GLint &model_uniform, int identifier);
    void setRandomPosition();
    void setControlPoints(Camera camera);
    void goToPlayer(Camera camera);
    glm::vec3 getSize();
    glm::vec3 getCenter();
};

#endif