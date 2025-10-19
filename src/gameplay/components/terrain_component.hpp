#pragma once
#include "mesh_component.hpp"

struct TerrainComponent {
    float maxHeight = 3.0f;
    glm::ivec2 dimensions = glm::ivec2(200);
    glm::ivec2 resolution = glm::ivec2(10);

    Texture heightMapTexture;
    Texture perlinNoiseTexture;

    int grassBlades = 500000;
};
