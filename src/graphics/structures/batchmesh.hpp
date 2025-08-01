#pragma once
#include <vector>

#include "fwd.hpp"
#include "mesh.hpp"

struct Batchmesh {
    Mesh mesh;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> rotations;
    std::vector<glm::vec3> scales;
    std::vector<glm::mat4> transforms;
};
