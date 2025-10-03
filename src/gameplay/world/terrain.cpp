#include "terrain.hpp"

#include <string>

#include "../../globals.hpp"

// Terrain::Generate - safe, explicit
void Terrain::Generate(int width = 100, int height = 100) {
    meshes.clear();
    meshes.emplace_back();
    Mesh &m = meshes[0];

    m.vertices.clear();
    m.indices.clear();
    m.uvs.clear();
    m.normals.clear();

    // reserve to avoid reallocation issues
    const int vertexCount = width * height;
    m.vertices.reserve(vertexCount * 3);
    m.uvs.reserve(vertexCount * 2);
    m.normals.reserve(vertexCount * 3);
    m.indices.reserve((width - 1) * (height - 1) * 6);

    // Build vertex positions and uvs
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            float x = static_cast<float>(i);
            float y = 0.0f; // replace with noise for actual terrain
            float z = static_cast<float>(j);
            m.vertices.push_back(x);
            m.vertices.push_back(y);
            m.vertices.push_back(z);

            // uv 0..1 over grid
            m.uvs.push_back(static_cast<float>(i) / (width - 1));
            m.uvs.push_back(static_cast<float>(j) / (height - 1));

            // placeholder normal (will be overwritten if you compute smooth normals)
            m.normals.push_back(0.0f);
            m.normals.push_back(1.0f);
            m.normals.push_back(0.0f);
        }
    }

    // Build indices: two triangles per quad
    for (int i = 0; i < width - 1; ++i) {
        for (int j = 0; j < height - 1; ++j) {
            unsigned int topLeft     = static_cast<unsigned int>(i * width + j);
            unsigned int topRight    = topLeft + 1;
            unsigned int bottomLeft  = static_cast<unsigned int>((i + 1) * width + j);
            unsigned int bottomRight = bottomLeft + 1;

            // Correct terrain indices for CCW (OpenGL default)
            m.indices.push_back(topLeft);
            m.indices.push_back(bottomRight);
            m.indices.push_back(bottomLeft);

            m.indices.push_back(topLeft);
            m.indices.push_back(topRight);
            m.indices.push_back(bottomRight);
        }
    }

    // Basic validation: ensure indices are inside range
    const unsigned int maxIndex = (unsigned int)(m.vertices.size() / 3);
    for (size_t k = 0; k < m.indices.size(); ++k) {
        if (m.indices[k] >= maxIndex) {
            // you can log or throw; prefer logging so you can see what's wrong without crashing
            globals.logger.ThrowRuntimeError("Terrain::Generate: index out of range at index " + std::to_string(k) +
                                            " value: " + std::to_string(m.indices[k]) +
                                            " max allowed: " + std::to_string(maxIndex - 1));
        }
    }

    Renderer::CreateMeshBuffers(meshes[0]);
    meshes[0].material.shaderProgramId = globals.renderer.opaqueLitShader.programId;
}
