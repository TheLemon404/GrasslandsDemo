#pragma once
#include "../../graphics/structures/multimesh.hpp"

struct Terrain : Multimesh {
    void Generate(int width, int height);
};
