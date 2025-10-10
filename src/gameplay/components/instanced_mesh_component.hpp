#pragma once
#include "../../graphics/structures/mesh.hpp"

struct InstancedMeshComponent {
    std::vector<Transform> transforms;
    unsigned int instancedVBO;
    Mesh mesh;
};
