#pragma once
#include "../../graphics/structures/skybox.hpp"
#include "vec3.hpp"

struct Environment {
    glm::vec3 sunDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 clearColor = glm::vec3(0.9f);
    glm::vec3 warmSun = glm::vec3(1.01, 0.95, 0.9);
    glm::vec3 coolSky = glm::vec3(1.01, 0.95, 0.9);
    Skybox skybox;
};
