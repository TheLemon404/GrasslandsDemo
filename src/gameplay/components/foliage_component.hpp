#pragma once

#include "../../graphics/structures/shader.hpp"
#include "../../graphics/structures/mesh.hpp"
#include "ext/vector_int2.hpp"
#include <string>

struct ChunkData {
    const glm::ivec2 numInstancesPerAxis = glm::ivec2(750);
    const glm::ivec2 chunkDimensions = glm::ivec2(1000);
    const glm::ivec2 chunkCenterCutout = glm::ivec2(0);
    const float clipDistance = 1000.0f;
};

struct FoliageComponent {
    std::string meshFile;
    //must be a perfect square
    ChunkData chunkData;

    float breezeAmount = 0.25f;
    float windAmount = 15.0f;
    float windAngle = 0.0f;

    ComputeShader foliagePlacementComputeShader;

    unsigned int instanceSSBO;
};
