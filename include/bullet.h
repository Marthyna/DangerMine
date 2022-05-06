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
    glm::vec3 position;
    GLint model_uniform;
    GLint object_id_uniform;
    int identifier;
    std::map<std::string, SceneObject> g_VirtualScene;
    GLint bbox_max_uniform;
    GLint bbox_min_uniform;
    Bullet();
    void initialize(GLint model_uniform, GLint object_id_uniform, int identifier, std::map<std::string, SceneObject> g_VirtualScene, GLint bbox_max_uniform, GLint bbox_min_uniform);
    void draw();
    void setPosition(glm::vec3 position);
};

#endif
