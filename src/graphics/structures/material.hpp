#pragma once
#include "shader.hpp"

struct Material {
    unsigned int shaderProgramId;

    glm::vec3 albedo = glm::vec3(1.0f, 1.0f, 1.0f);
};
