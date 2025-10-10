#pragma once
#include "mesh_component.hpp"

struct TerrainComponent {
    float maxHeight = 5.0f;
    glm::ivec2 dimensions = glm::ivec2(50);
    glm::ivec2 resolution = glm::ivec2(200);

    Texture heightMapTexture;

    int grassBlades = 100000;
};
