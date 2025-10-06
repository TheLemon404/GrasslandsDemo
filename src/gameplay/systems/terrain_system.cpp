#include "terrain_system.hpp"

#include "../../globals.hpp"
#include "../components/mesh_component.hpp"
#include "../components/terrain_mesh_component.hpp"
#include "glad/glad.h"

void TerrainSystem::Start(entt::registry& registry) {
    auto view = registry.view<MeshComponent, TerrainMeshComponent>();
    for (auto& entity : view) {
        TerrainMeshComponent& terrain = registry.get<TerrainMeshComponent>(entity);
        Mesh& mesh = registry.get<MeshComponent>(entity).mesh;
        mesh.vertices.clear();
        mesh.indices.clear();
        mesh.uvs.clear();
        mesh.normals.clear();

        // Build vertex positions and uvs
        for (int i = 0; i < terrain.resolution.x; ++i) {
            for (int j = 0; j < terrain.resolution.y; ++j) {
                float x = static_cast<float>(i) - terrain.resolution.x / 2;
                float z = static_cast<float>(j) - terrain.resolution.y / 2;
                mesh.vertices.push_back({x * ((float)terrain.dimensions.x / terrain.resolution.x), 0.0f, z * ((float)terrain.dimensions.y / terrain.resolution.y)});

                //this is not actually used in the shader (we calculate the normals again in the fragment shader
                mesh.normals.push_back({0.0f, 1.0f, 0.0f});

                // uv 0..1 over grid
                mesh.uvs.push_back({static_cast<float>(i) / (terrain.resolution.x - 1), static_cast<float>(j) / (terrain.resolution.y - 1)});
            }
        }

        // Build indices: two triangles per quad
        for (int i = 0; i < terrain.resolution.x - 1; ++i) {
            for (int j = 0; j < terrain.resolution.y - 1; ++j) {
                unsigned int topLeft     = static_cast<unsigned int>(i * terrain.resolution.y + j);
                unsigned int topRight    = topLeft + 1;
                unsigned int bottomLeft  = static_cast<unsigned int>((i + 1) * terrain.resolution.y + j);
                unsigned int bottomRight = bottomLeft + 1;

                // Correct terrain indices for CCW (OpenGL default)
                mesh.indices.push_back(topLeft);
                mesh.indices.push_back(bottomRight);
                mesh.indices.push_back(bottomLeft);

                mesh.indices.push_back(topLeft);
                mesh.indices.push_back(topRight);
                mesh.indices.push_back(bottomRight);
            }
        }

        // Basic validation: ensure indices are inside range
        const unsigned int maxIndex = (unsigned int)(mesh.vertices.size());
        for (size_t k = 0; k < mesh.indices.size(); ++k) {
            if (mesh.indices[k] >= maxIndex) {
                // you can log or throw; prefer logging so you can see what's wrong without crashing
                globals.logger.ThrowRuntimeError("Terrain::Generate: index out of range at index " + std::to_string(k) +
                                                " value: " + std::to_string(mesh.indices[k]) +
                                                " max allowed: " + std::to_string(maxIndex - 1));
            }
        }

        mesh.cullBackface = false;

        terrain.heightMapTexture = Texture::LoadTextureFromFile("resources/textures/th.png", 3);

        Renderer::CreateMeshBuffers(mesh);
        mesh.material.shaderProgramId = globals.renderer.terrainShader.programId;
        mesh.material.albedo = glm::vec3(0.4f, 0.9f, 0.5f);
        mesh.material.roughness = 1.0f;
    }
}

void TerrainSystem::Update(entt::registry& registry) {

}

void TerrainSystem::InsertDrawLogic(Mesh& mesh, entt::entity& entity) {
    if (globals.scene.registry.try_get<TerrainMeshComponent>(entity)) {
        TerrainMeshComponent terrain = globals.scene.registry.get<TerrainMeshComponent>(entity);

        Renderer::UploadShaderUniformInt(mesh.material.shaderProgramId, "heightMap", 0);
        Renderer::UploadShaderUniformFloat(mesh.material.shaderProgramId, "heightMapStrength", terrain.maxHeight);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrain.heightMapTexture.id);
    }
}

