#pragma once

#include <glm.hpp>

struct Camera {
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 position = glm::vec3(4.0f, 4.0f, 4.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float fov = 70.0f;
    float near = 0.5f;
    float far = 2000.0f;

    glm::mat4 view;
    glm::mat4 projection;
};
