#pragma once

#include "../../graphics/structures/shader.hpp"
#include "../../graphics/structures/mesh.hpp"
#include "ext/vector_int2.hpp"

struct FoliageComponent {
    //must be a perfect square
    glm::ivec2 numInstancesPerAxis = glm::ivec2(500);
    glm::ivec2 foliageChunkDimensions = glm::ivec2(250);
    float cameraCutoffDistance = 500.0f;
    float breezeAmount = 0.25f;
    float windAmount = 25.0f;
    float windAngle = 0.0f;

    ComputeShader foliagePlacementComputeShader;

    unsigned int instancedSSBO;
};
