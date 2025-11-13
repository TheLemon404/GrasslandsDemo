#pragma once
#include "../../graphics/structures/skybox.hpp"
#include "vec3.hpp"

struct Environment {
    glm::vec3 sunDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 ambientColor = glm::vec3(1.0f, 0.95f, 0.85f);
    glm::vec3 clearColor = glm::vec3(0.9f);
    Skybox skybox;
};
