#pragma once
#include "mesh_component.hpp"

struct TerrainMeshComponent {
    float heightScale = 1.0f;
    float flatScale = 1.0f;
    int width = 50, height = 50;
};
