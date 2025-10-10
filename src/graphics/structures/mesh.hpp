#pragma once
#include "material.hpp"

struct Mesh {
    Material material;
    unsigned int vao, vbo, nbo, uvbo, ibo;
    bool cullBackface = true;
    int shadowCullFace = 0;
    bool castsShadow = true;
    bool receivesShadow = true;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    std::vector<float> GetVerticesRaw() {
        std::vector<float> result;
        for (int i = 0; i < vertices.size(); i++) {
            result.push_back(vertices[i].x);
            result.push_back(vertices[i].y);
            result.push_back(vertices[i].z);
        }
        return result;
    }

    std::vector<float> GetNormalsRaw() {
        std::vector<float> result;
        for (int i = 0; i < normals.size(); i++) {
            result.push_back(normals[i].x);
            result.push_back(normals[i].y);
            result.push_back(normals[i].z);
        }
        return result;
    }

    std::vector<float> GetUvsRaw() {
        std::vector<float> result;
        for (int i = 0; i < uvs.size(); i++) {
            result.push_back(uvs[i].x);
            result.push_back(uvs[i].y);
        }
        return result;
    }
};
