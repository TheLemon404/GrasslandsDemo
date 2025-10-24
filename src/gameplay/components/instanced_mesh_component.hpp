#pragma once
#include "../../graphics/structures/mesh.hpp"
#include "../../graphics/structures/transform.hpp"

struct InstancedMeshComponent {
    std::vector<Transform> transforms;
    unsigned int instancedSSBO;
    Mesh mesh;
};
