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

class Enemy
{
public:
    glm::vec3 position;
    GLint model_uniform;
    GLint object_id_uniform;
    int identifier;
    std::map<std::string, SceneObject> g_VirtualScene;
    GLint bbox_max_uniform;
    GLint bbox_min_uniform;
    Enemy(glm::vec4 center_point);
    
    void initialize(
        GLint model_uniform, 
        GLint object_id_uniform, 
        int identifier, 
        std::map<std::string, SceneObject> g_VirtualScene, 
        GLint bbox_max_uniform, 
        GLint bbox_min_uniform);
    void draw();
    void setPosition(glm::vec3 position);
};

#endif