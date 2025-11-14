#pragma once
#include "ext/vector_int2.hpp"
#include "mesh_component.hpp"

struct TerrainComponent {
    float maxHeight = 25.0f;
    const glm::ivec2 dimensions = glm::ivec2(750);
    const glm::ivec2 resolution = glm::ivec2(75);

    Texture heightMapTexture;
    Texture perlinNoiseTexture;

    const int grassBlades = 500000;
};
