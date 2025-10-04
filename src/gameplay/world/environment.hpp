#pragma once
#include "vec3.hpp"

struct Environment {
    glm::vec3 sunDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 sunColor = glm::vec3(0.9f, 0.88f, 0.9f);
    glm::vec3 shadowColor = glm::vec3(0.35f, 0.3f, 0.35f);
    glm::vec3 clearColor = glm::vec3(0.9f);
};
