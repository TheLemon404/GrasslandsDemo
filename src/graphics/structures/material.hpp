#pragma once
#include <memory>

#include "texture.hpp"
#include "glm.hpp"
#include "shader.hpp"

struct Material {
    std::shared_ptr<Shader> shader = nullptr;

    glm::vec3 albedo = glm::vec3(1.0f, 1.0f, 1.0f);
    float roughness = 0.5f;
    Texture texture = {};
};
