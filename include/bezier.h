#ifndef _BEZIER_H
#define _BEZIER_H

#include <cstdio>
#include <cstdlib>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 cubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t);

#endif