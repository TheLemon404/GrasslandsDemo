#pragma once
#include <memory>

#include "../../graphics/structures/mesh.hpp"
#include "../../graphics/structures/transform.hpp"

struct Terrain {
    Mesh mesh;
    Transform transform;
};
