#pragma once
#include <vector>

#include "fwd.hpp"
#include "mesh.hpp"
#include "transform.hpp"

struct Batchmesh {
    Mesh mesh;
    std::vector<Transform> transforms;
};
