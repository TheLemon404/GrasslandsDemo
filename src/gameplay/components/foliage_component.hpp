#pragma once

#include "../../graphics/structures/shader.hpp"
#include "../../graphics/structures/mesh.hpp"

struct FoliageComponent {
    //must be a perfect square
    glm::ivec2 numInstancesPerAxis = glm::ivec2(100, 100);
    float cameraCutoffDistance = 200.0f;
    float breezeAmount = 0.25f;
    float windAmount = 25.0f;
    float windAngle = 0.0f;

    ComputeShader foliagePlacementComputeShader;

    unsigned int instancedSSBO;
};
