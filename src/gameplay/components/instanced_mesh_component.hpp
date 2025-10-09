#pragma once
#include "../../graphics/structures/mesh.hpp"

struct InstancedMeshComponent {
    std::vector<Transform> transforms;
    Mesh mesh;
};
