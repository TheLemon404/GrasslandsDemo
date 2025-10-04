#pragma once
#include "material.hpp"

struct Mesh {
    Material material;
    unsigned int vao, vbo, nbo, uvbo, ibo;
    bool cullBackface = true;
    int shadowCullFace = 0;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<unsigned int> indices;
};
