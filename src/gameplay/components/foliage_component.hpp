#pragma once

#include "../../graphics/structures/shader.hpp"
#include "../../graphics/structures/mesh.hpp"
#include "ext/vector_int2.hpp"

struct ChunkData {
    glm::ivec2 numInstancesPerAxis = glm::ivec2(500);
    glm::ivec2 chunkDimensions = glm::ivec2(250);
    float clipDistance = 500.0f;
};

struct FoliageComponent {
    //must be a perfect square
    ChunkData lod0ChunkData;
    ChunkData lod1ChunkData = {
        glm::ivec2(1000),
        glm::ivec2(500),
        1000.0f
    };
    float breezeAmount = 0.25f;
    float windAmount = 25.0f;
    float windAngle = 0.0f;

    ComputeShader foliagePlacementComputeShader;

    unsigned int lod0instanceSSBO;
    unsigned int lod1instanceSSBO;
};
