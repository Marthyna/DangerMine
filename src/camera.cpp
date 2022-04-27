#include "camera.h"

Camera::Camera(GLuint program_id)
{
    Camera::program_id = program_id;

    g_CameraTheta = 0.f;
    g_CameraPhi = 0.f;
    g_CameraDistance = 6.5f;

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

void Camera::listenForInputs(GLFWwindow *window, double *mouseXPos, double *mouseYPos, double *mouseXOffset, double *mouseYOffset)
{
    // g_CameraDistance -= 0.1f * (*mouseYOffset);

    // const float verysmallnumber = std::numeric_limits<float>::epsilon();
    // if (g_CameraDistance < verysmallnumber)
    //     g_CameraDistance = verysmallnumber;

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
    // else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    // {
    //     // Unhides cursor since camera is not looking around anymore
    //     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //     // Makes sure the next time the camera looks around it doesn't jump
    //     firstClick = true;
    // }

    // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    // {

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    double mouseX, mouseY;

    glfwGetCursorPos(window, &mouseX, &mouseY);

    float dx = *mouseXPos - mouseX;
    float dy = *mouseYPos - mouseY;

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