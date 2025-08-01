#pragma once
#include <vector>
#include "mesh.hpp"
#include "transform.hpp"

struct Multimesh {
    Transform transform;

    std::vector<Mesh> meshes;
};
