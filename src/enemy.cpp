#include "enemy.h"
#include "helpers.h"

Enemy::Enemy()
{
    this->t = 0.0f;
}

void Enemy::draw(Camera camera, std::map<std::string, SceneObject> &g_VirtualScene, glm::mat4 &model, GLint &bbox_max_uniform, GLint &bbox_min_uniform, GLint &object_id_uniform, GLint &model_uniform, int identifier)
{
    this->goToPlayer(camera);

    model = Matrix_Translate(this->position[0], this->position[1], this->position[2]) * Matrix_Scale(0.0025f, 0.0025f, 0.0025f);

    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(object_id_uniform, identifier);

    glBindVertexArray(g_VirtualScene["enemy"].vertex_array_object_id);

    glm::vec3 bbox_min = g_VirtualScene["enemy"].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene["enemy"].bbox_max;
    this->bbox_max = bbox_max + this->position;
    this->bbox_min = bbox_min + this->position;
    glUniform4f(bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    glDrawElements(
        g_VirtualScene["enemy"].rendering_mode,
        g_VirtualScene["enemy"].num_indices,
        GL_UNSIGNED_INT,
        (void *)(g_VirtualScene["enemy"].first_index * sizeof(GLuint)));

    glBindVertexArray(0);
}

void Enemy::setRandomPosition()
{
    float x = RandomFloat(0, 10);
    float y = RandomFloat(0, 4);
    float z = RandomFloat(-8, 8);

    this->position = glm::vec3(x, y, z);
}

void Enemy::setControlPoints(Camera camera)
{
    float xHalfWay = (this->position[0] - camera.center_point[0]) / 2;
    float yHalfWay = (this->position[1] - camera.center_point[1]) / 2;
    float zHalfWay = (this->position[2] - camera.center_point[2]) / 2;

    this->p1 = glm::vec3(xHalfWay, yHalfWay - 0.5f, zHalfWay);
    this->p2 = glm::vec3(xHalfWay + 1.0f, yHalfWay, zHalfWay);
    this->p3 = glm::vec3(xHalfWay - 1.0f, yHalfWay, zHalfWay);
}

void Enemy::goToPlayer(Camera camera)
{
    this->position = cubicBezier(this->position, this->p1, this->p2, camera.center_point - 0.5f, this->t);
    this->t += 0.002f;
}

glm::vec3 Enemy::getSize()
{
    float x = this->bbox_max[0] - this->bbox_min[0] / 2;
    float y = this->bbox_max[1] - this->bbox_min[1] / 2;
    float z = this->bbox_max[2] - this->bbox_min[2] / 2;

    return glm::vec3(x, y, z);
}

glm::vec3 Enemy::getCenter()
{
    float x = (this->bbox_max[0] + this->bbox_min[0]) / 2;
    float y = (this->bbox_max[1] + this->bbox_min[1]) / 2;
    float z = (this->bbox_max[2] + this->bbox_min[2]) / 2;

    return glm::vec3(x, y, z);
}