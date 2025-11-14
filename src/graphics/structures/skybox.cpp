#include "./skybox.hpp"
#include "../../application.hpp"
#include <memory>
#include <vector>

float skyboxVertices[] = {
    // positions
    -1.0f, -1.0f, -1.0f,  // 0: left-bottom-back
     1.0f, -1.0f, -1.0f,  // 1: right-bottom-back
     1.0f,  1.0f, -1.0f,  // 2: right-top-back
    -1.0f,  1.0f, -1.0f,  // 3: left-top-back
    -1.0f, -1.0f,  1.0f,  // 4: left-bottom-front
     1.0f, -1.0f,  1.0f,  // 5: right-bottom-front
     1.0f,  1.0f,  1.0f,  // 6: right-top-front
    -1.0f,  1.0f,  1.0f   // 7: left-top-front
};

// Indices for 6 faces (2 triangles per face)
unsigned int skyboxIndices[] = {
    // Back face
    0, 1, 2,
    2, 3, 0,
    // Left face
    4, 0, 3,
    3, 7, 4,
    // Right face
    1, 5, 6,
    6, 2, 1,
    // Front face
    5, 4, 7,
    7, 6, 5,
    // Top face
    3, 2, 6,
    6, 7, 3,
    // Bottom face
    4, 5, 1,
    1, 0, 4
};

Skybox Skybox::LoadSkybox() {
    Skybox result = {};

    result.cubemapMesh = Mesh();
    result.cubemapMesh.indices = std::vector<unsigned int>(std::begin(skyboxIndices), std::end(skyboxIndices));
    result.cubemapMesh.material.shader = std::make_shared<Shader>(Application::Get()->renderer.skyboxShader);

    int v = 0;
    for(int i = 0; i < (sizeof(skyboxVertices) / sizeof(skyboxVertices[0])) / 3; i++) {
        result.cubemapMesh.vertices.push_back(glm::vec3(skyboxVertices[v], skyboxVertices[v + 1], skyboxVertices[v + 2]));
        v += 3;
    }

    Renderer::CreateMeshBuffers(result.cubemapMesh);

    return result;
}
