#include "bullet.h"
#include <chrono>
#include <thread>
#define GUN_ANGLE_Z -0.2f
#define GUN_ANGLE_Y -4.3f
#define GUN_ANGLE_X 0.39

Bullet::Bullet(glm::vec4 center_point, glm::vec4 view_vector)
{
    this->position = glm::vec4(center_point[0], center_point[1] - 0.2f, center_point[2], center_point[3]);
    this->cam_view_vector = view_vector;
}

void Bullet::draw(std::map<std::string, SceneObject> &g_VirtualScene, glm::mat4 model, GLint &bbox_max_uniform, GLint &bbox_min_uniform, GLint &object_id_uniform, GLint &model_uniform, int identifier, double tprev)
{
    double tnow = glfwGetTime();

    float deltaTime = tnow - tprev;
    tprev = tnow;

    glm::vec4 w = -cam_view_vector / norm(cam_view_vector);

    position -= (deltaTime / 100) * w;

    model = Matrix_Translate(this->position[0], this->position[1], this->position[2]) * Matrix_Scale(0.025f, 0.025f, 0.2f);

    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(object_id_uniform, identifier);

    glBindVertexArray(g_VirtualScene["bullet"].vertex_array_object_id);

    glm::vec3 bbox_min = g_VirtualScene["bullet"].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene["bullet"].bbox_max;
    this->bbox_max = bbox_max;
    this->bbox_min = bbox_min;
    glUniform4f(bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    glDrawElements(
        g_VirtualScene["bullet"].rendering_mode,
        g_VirtualScene["bullet"].num_indices,
        GL_UNSIGNED_INT,
        (void *)(g_VirtualScene["bullet"].first_index * sizeof(GLuint)));

    glBindVertexArray(0);
}

glm::vec3 Bullet::getSize()
{
    float x = this->bbox_max[0] - this->bbox_min[0] / 2;
    float y = this->bbox_max[1] - this->bbox_min[1] / 2;
    float z = this->bbox_max[2] - this->bbox_min[2] / 2;

    return glm::vec3(x, y, z);
}

glm::vec3 Bullet::getCenter()
{
    float x = (this->bbox_max.x + this->bbox_min.x) / 2;
    float y = (this->bbox_max.y + this->bbox_min.y) / 2;
    float z = (this->bbox_max.z + this->bbox_min.z) / 2;

    return glm::vec3(x, y, z);
}