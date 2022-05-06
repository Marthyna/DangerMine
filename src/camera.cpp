#include "camera.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <thread>

Camera::Camera(GLuint program_id)
{
    Camera::program_id = program_id;

    g_CameraTheta = 0.f;
    g_CameraPhi = 0.f;
    g_CameraDistance = 10.0f;

    lookat_point = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    center_point = glm::vec4(0.0f, 0.0f, 2.5f, 1.0f);
    view_vector = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    up_vector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
};

void Camera::update()
{
    view = Matrix_Camera_View(center_point, view_vector, up_vector);

    glUniformMatrix4fv(glGetUniformLocation(program_id, "view"), 1, GL_FALSE, glm::value_ptr(view));
};

void Camera::listenForInputs(GLFWwindow *window, double *mouseXPos, double *mouseYPos, double *mouseXOffset, double *mouseYOffset, bool isColliding, std::vector<Bullet> &bullets)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        glm::vec4 w;
        if (isColliding)
        {
            glm::vec4 front = glm::vec4(view_vector[0], 0.0f, view_vector[2], 0.0f);

            w = -front / norm(front);

            center_point -= 0.01f * w;
        }
        else
        {
            w = -view_vector / norm(view_vector);
        }
        center_point -= 0.01f * w;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        glm::vec4 w = -view_vector / norm(view_vector);
        center_point -= 0.01f * w;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        glm::vec4 w = -view_vector / norm(view_vector);
        center_point += 0.01f * w;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        glm::vec4 w = -view_vector / norm(view_vector);
        center_point += 0.01f * (crossproduct(up_vector, w));
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        glm::vec4 w = -view_vector / norm(view_vector);
        center_point -= 0.01f * (crossproduct(up_vector, w));
    }
    static int oldState = GLFW_RELEASE;
    int newState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    if (newState == GLFW_RELEASE && oldState == GLFW_PRESS)
    {
        Bullet bullet(this->center_point, this->view_vector);
        // bullet.setPosition(glm::vec3(this->center_point[0], this->center_point[1], this->center_point[2]));
        // bullet.draw();
        bullets.push_back(bullet);
    }
    oldState = newState;

    double mouseX, mouseY;

    glfwGetCursorPos(window, &mouseX, &mouseY);

    float dx = *mouseXPos - mouseX;
    float dy = *mouseYPos - mouseY;

    if (dx == 0 && dy == 0)
    {
        return;
    }

    g_CameraTheta += 0.01f * dx;
    g_CameraPhi += 0.01f * dy;

    float phimax = 3.141592f / 2;
    float phimin = -phimax;

    if (g_CameraPhi > phimax)
        g_CameraPhi = phimax;

    if (g_CameraPhi < phimin)
        g_CameraPhi = phimin;

    mouseX = *mouseXPos;
    mouseY = *mouseYPos;

    y = g_CameraDistance * sin(g_CameraPhi);
    z = g_CameraDistance * cos(g_CameraPhi) * cos(g_CameraTheta);
    x = g_CameraDistance * cos(g_CameraPhi) * sin(g_CameraTheta);

    lookat_point = glm::vec4(x, y, z - 1.0f, 1.0f);
    view_vector = lookat_point - center_point;

    // }
}