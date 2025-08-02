#pragma once
#include "vec3.hpp"

struct Environment {
    glm::vec3 sunDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 sunColor = glm::vec3(0.9f, 0.85f, 0.7f);
    glm::vec3 shadowColor = glm::vec3(0.35f, 0.3f, 0.35f);
    glm::vec3 clearColor = glm::vec3(0.9f, 0.7f, 0.6f);
};
