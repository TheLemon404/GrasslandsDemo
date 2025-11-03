#pragma once

#include "../../graphics/structures/shader.hpp"
#include "../../graphics/structures/mesh.hpp"
#include "ext/vector_int2.hpp"
#include <string>

struct ChunkData {
    glm::ivec2 numInstancesPerAxis = glm::ivec2(1250);
    glm::ivec2 chunkDimensions = glm::ivec2(1000);
    glm::ivec2 chunkCenterCutout = glm::ivec2(0);
    float clipDistance = 1000.0f;
};

struct FoliageComponent {
    std::string meshFile;
    //must be a perfect square
    ChunkData chunkData;

    float breezeAmount = 0.25f;
    float windAmount = 25.0f;
    float windAngle = 0.0f;

    ComputeShader foliagePlacementComputeShader;

    unsigned int instanceSSBO;
};
