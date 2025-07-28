#pragma once
#include "material.hpp"

struct Mesh {
    Material material;
    unsigned int vao, vbo, ibo;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<unsigned int> indices;
};
