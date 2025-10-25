#pragma once
#include "ext/vector_int2.hpp"
#include "mesh_component.hpp"

struct TerrainComponent {
    float maxHeight = 25.0f;
    glm::ivec2 dimensions = glm::ivec2(1000);
    glm::ivec2 resolution = glm::ivec2(50);

    Texture heightMapTexture;
    Texture perlinNoiseTexture;

    int grassBlades = 500000;
};
