#pragma once

struct DrawElementsIndirectCommand {
    unsigned int count;          // number of indices per instance
    unsigned int instanceCount;  // number of instances to draw
    unsigned int firstIndex;     // offset in index buffer
    unsigned int baseVertex;     // base vertex in vertex buffer
    unsigned int baseInstance;   // starting instance ID
};

struct FoliageComponent {
    //must be a perfect square
    glm::ivec2 numPatches = glm::ivec2(3);
    float cameraCutoffDistance = 200.0f;
    float breezeAmount = 0.25f;
    float windAmount = 25.0f;
    float windAngle = 0.0f;

    ComputeShader foliagePlacementComputeShader;

    unsigned int instancedSSBO;
    unsigned int indirectDrawCommandSSBO;
    Mesh meshLOD0;
};