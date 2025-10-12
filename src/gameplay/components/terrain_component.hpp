#pragma once
#include "mesh_component.hpp"

struct TerrainComponent {
    float maxHeight = 3.0f;
    glm::ivec2 dimensions = glm::ivec2(300);
    glm::ivec2 resolution = glm::ivec2(200);

    Texture heightMapTexture;
    Texture perlinNoiseTexture;

    int numGrassSectorsPerSide = 3;
    std::vector<glm::vec2> grassSectors;

    int grassBlades = 3000000;
};
