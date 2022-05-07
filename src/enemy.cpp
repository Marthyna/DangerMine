#include "enemy.h"
#include <chrono>
#include <thread>

Enemy::Enemy(glm::vec4 center_point)
{
    this->position = glm::vec3(center_point[0], center_point[1], center_point[2]);
}

void Enemy::initialize(
    GLint model_uniform, 
    GLint object_id_uniform, 
    int identifier, 
    std::map<std::string, SceneObject> g_VirtualScene, 
    GLint bbox_max_uniform, 
    GLint bbox_min_uniform)
{
    this->g_VirtualScene = g_VirtualScene;
    this->identifier = identifier;
    this->model_uniform = model_uniform;
    this->bbox_max_uniform = bbox_max_uniform;
    this->bbox_min_uniform = bbox_min_uniform;
}

void Enemy::draw()
{
    glm::mat4 model = Matrix_Translate(this->position[0], this->position[1], this->position[2] += 0.1f) * Matrix_Scale(0.025f, 0.025f, 0.2f);

    glUniformMatrix4fv(this->model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(this->object_id_uniform, this->identifier);

    glBindVertexArray(g_VirtualScene["enemy"].vertex_array_object_id);

    glm::vec3 bbox_min = g_VirtualScene["enemy"].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene["enemy"].bbox_max;
    glUniform4f(bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    glDrawElements(
        g_VirtualScene["enemy"].rendering_mode,
        g_VirtualScene["enemy"].num_indices,
        GL_UNSIGNED_INT,
        (void *)(g_VirtualScene["enemy"].first_index * sizeof(GLuint)));

    glBindVertexArray(0);
}