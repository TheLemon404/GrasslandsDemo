#pragma once
#include "vec3.hpp"

struct Environment {
    glm::vec3 sunDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 ambientColor = glm::vec3(0.85, 0.90, 0.95);
    glm::vec3 shadowColor = glm::vec3(0.3f, 0.3f, 0.3f);
    glm::vec3 clearColor = glm::vec3(0.9f);
};
