#pragma once
#include "texture.hpp"
#include "glm.hpp"

struct Material {
    unsigned int shaderProgramId;

    glm::vec3 albedo = glm::vec3(1.0f, 1.0f, 1.0f);
    float specular = 0.5f;
    Texture texture = {};
};
