#ifndef _BULLET_CLASS_H
#define _BULLET_CLASS_H

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

class Bullet
{
public:
    glm::vec4 position;
    glm::vec4 cam_view_vector;
    glm::vec3 bbox_max;
    glm::vec3 bbox_min;

    Bullet(glm::vec4 center_point, glm::vec4 view_vector);
    void draw(std::map<std::string, SceneObject> &g_VirtualScene, glm::mat4 model, GLint &bbox_max_uniform, GLint &bbox_min_uniform, GLint &object_id_uniform, GLint &model_uniform, int identifier, double tprev);
    void setPosition(glm::vec3 position);
    glm::vec3 getSize();
    glm::vec3 getCenter();
};

#endif
