#pragma once
#include "../../graphics/structures/mesh.hpp"
#include "../../graphics/structures/transform.hpp"

struct InstancedMeshComponent {
    std::vector<Transform> transforms;
    unsigned int instanceSSBO;
    int ssboBinding = 1;
    Mesh mesh;
};
