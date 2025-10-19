#pragma once

struct FoliageComponent {
    int numInstances = 150000;
    float cameraCutoffDistance = 200.0f;
    float breezeAmount = 0.25f;
    float windAmount = 25.0f;
    float windAngle = 0.0f;
    glm::vec2 dimensions = glm::vec2(100);

    ComputeShader foliagePlacementComputeShader;
};