#pragma once
#include "fwd.hpp"
#include "ext/matrix_transform.hpp"

struct Transform {
    glm::vec3 position = {};
    glm::vec3 rotation = {};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    glm::mat4 matrix = glm::identity<glm::mat4>();
};
