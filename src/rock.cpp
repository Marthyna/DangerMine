#include "rock.h"

Rock::Rock()
{
    float x = RandomFloat(-8.0f, 8.0f);
    float y = -1.0f;
    float z = RandomFloat(-10.0f, 10.0f);

    while (z > -1.0f && z < 1.0f)
    {
        fprintf(stderr, "%f \n", z);
        z = RandomFloat(-10.0f, 10.0f);
    }

    this->position = {x, y, z};
    this->size = RandomFloat(0.02, 0.035);
    this->rotateY = RandomFloat(-2, 0.75);
}

void Rock::draw(std::map<std::string, SceneObject> &g_VirtualScene, glm::mat4 model, GLint &bbox_max_uniform, GLint &bbox_min_uniform, GLint &object_id_uniform, GLint &model_uniform, int identifier)
{
    model = Matrix_Rotate_Y(this->rotateY) * Matrix_Translate(this->position[0], this->position[1], this->position[2]) * Matrix_Scale(this->size, this->size, this->size);

    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(object_id_uniform, identifier);

    glBindVertexArray(g_VirtualScene["rock"].vertex_array_object_id);

    glm::vec3 bbox_min = g_VirtualScene["rock"].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene["rock"].bbox_max;
    this->bbox_max = bbox_max + this->position;
    this->bbox_min = bbox_min + this->position;
    glUniform4f(bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    glDrawElements(
        g_VirtualScene["rock"].rendering_mode,
        g_VirtualScene["rock"].num_indices,
        GL_UNSIGNED_INT,
        (void *)(g_VirtualScene["rock"].first_index * sizeof(GLuint)));

    glBindVertexArray(0);
}