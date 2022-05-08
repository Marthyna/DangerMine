#ifndef _CAMERA_CLASS_H
#define _CAMERA_CLASS_H

#include "matrices.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <glad/glad.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "bullet.h"
#include <vector>

class Camera
{
private:
    GLuint program_id;

public:
    float y;
    float z;
    float x;

    float g_CameraDistance;

    float g_CameraTheta;
    float g_CameraPhi;

    glm::vec4 center_point;
    glm::vec4 lookat_point;
    glm::vec4 view_vector;
    glm::vec4 up_vector;
    glm::mat4 view;

    Camera(GLuint program_id);
    void update();
    void listenForInputs(GLFWwindow *window, 
                         double *mouseXPos, 
                         double *mouseYPos, 
                         double *mouseXOffset, 
                         double *mouseYOffset, 
                         bool isCollidingWithGround,
                         bool isCollidingWithRock, 
                         std::vector<Bullet> &bullets, 
                         int g_chosenTool);
};

#endif
