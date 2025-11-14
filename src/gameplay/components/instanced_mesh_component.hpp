#pragma once
#include "../../graphics/structures/mesh.hpp"
#include "../../graphics/structures/transform.hpp"

struct InstancedMeshComponent {
    std::vector<Transform> transforms;
    std::vector<glm::mat4> matrices;
    unsigned int instanceSSBO;
    unsigned int instanceVBO;
    const int ssboBinding = 0;
    Mesh mesh;
};
