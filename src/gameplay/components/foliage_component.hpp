#pragma once

#include "../../graphics/structures/shader.hpp"
#include "../../graphics/structures/mesh.hpp"
#include "ext/vector_int2.hpp"

struct FoliageComponent {
    //must be a perfect square
    glm::ivec2 numInstancesPerAxis = glm::ivec2(400);
    glm::ivec2 foliageChunkDimensions = glm::ivec2(200);
    float cameraCutoffDistance = 200.0f;
    float breezeAmount = 0.25f;
    float windAmount = 25.0f;
    float windAngle = 0.0f;

    ComputeShader foliagePlacementComputeShader;

    unsigned int instancedSSBO;
};
