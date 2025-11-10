#pragma once
#include <memory>

#include "texture.hpp"
#include "shader.hpp"

struct Material {
    std::shared_ptr<Shader> shader = nullptr;

    glm::vec3 albedo = glm::vec3(1.0f);
    glm::vec3 shadowColor = glm::vec3(0.4f);
    float roughness = 0.5f;
    float shininess = 3.0f;
    Texture texture = {};
    bool applyWind = false;
};
