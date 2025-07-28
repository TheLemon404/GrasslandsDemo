#pragma once
#include <vector>
#include "mesh.hpp"

struct Multimesh {
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 rotation = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);
    glm::mat4 transform;

    std::vector<Mesh> meshes;
};
