#include "player.h"

Player::Player()
{
    this->position = {0.45f, -0.5f, -0.8f};
    this->size = 0.08f;
    this->rotateX = 0.39;
    this->rotateY = -1.3f;
    this->rotateZ = -0.2f;
}

void Player::draw(
    std::map<std::string, SceneObject> &g_VirtualScene,
    glm::mat4 &model,
    GLint &bbox_max_uniform,
    GLint &bbox_min_uniform,
    GLint &object_id_uniform,
    GLint &model_uniform,
    int identifier,
    glm::mat4 inverse,
    Camera camera)
{
    model = inverse * Matrix_Translate(this->position[0], this->position[1], this->position[2]) * Matrix_Scale(this->size, this->size, this->size) * Matrix_Rotate_Z(this->rotateX) * Matrix_Rotate_Y(this->rotateY) * Matrix_Rotate_X(this->rotateZ);

    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(object_id_uniform, identifier);

    glBindVertexArray(g_VirtualScene["gun"].vertex_array_object_id);

    glm::vec3 bbox_min = g_VirtualScene["gun"].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene["gun"].bbox_max;
    this->bbox_max = bbox_max;
    this->bbox_min = bbox_min;
    glUniform4f(bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    glDrawElements(
        g_VirtualScene["gun"].rendering_mode,
        g_VirtualScene["gun"].num_indices,
        GL_UNSIGNED_INT,
        (void *)(g_VirtualScene["gun"].first_index * sizeof(GLuint)));

    glBindVertexArray(0);
}