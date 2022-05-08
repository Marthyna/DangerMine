#include "bezier.h"

glm::vec3 cubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
    float x = pow(1 - t, 3) * p0[0] +
              pow(1 - t, 2) * 3 * t * p1[0] +
              (1 - t) * 3 * t * t * p2[0] +
              t * t * t * p3[0];

    float y = pow(1 - t, 3) * p0[1] +
              pow(1 - t, 2) * 3 * t * p1[1] +
              (1 - t) * 3 * t * t * p2[1] +
              t * t * t * p3[1];

    float z = pow(1 - t, 3) * p0[2] +
              pow(1 - t, 2) * 3 * t * p1[2] +
              (1 - t) * 3 * t * t * p2[2] +
              t * t * t * p3[2];

    return glm::vec3(x, y, z);
}