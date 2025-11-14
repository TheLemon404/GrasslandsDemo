#include "terrain_system.hpp"

#include "../../application.hpp"
#include "../components/mesh_component.hpp"
#include "../components/terrain_component.hpp"
#include "entt/entity/fwd.hpp"
#include "glad/glad.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/rotate_vector.hpp"

void TerrainSystem::Start(entt::registry& registry) {
    auto view = registry.view<MeshComponent, TerrainComponent>();
    for (auto& entity : view) {
        TerrainComponent& terrain = registry.get<TerrainComponent>(entity);
        Mesh& mesh = registry.get<MeshComponent>(entity).mesh;
        mesh.vertices.clear();
        mesh.indices.clear();
        mesh.uvs.clear();
        mesh.normals.clear();

        // Build vertex positions and uvs
        for (int i = 0; i < terrain.resolution.x; ++i) {
            for (int j = 0; j < terrain.resolution.y; ++j) {
                float x = static_cast<float>(i) / ((float)terrain.resolution.x - 1);
                float z = static_cast<float>(j) / ((float)terrain.resolution.y - 1);

                glm::vec3 position = glm::vec3(x, 0, z) * glm::vec3((float)terrain.dimensions.x, 0, (float)terrain.dimensions.y);
                position -= glm::vec3(terrain.dimensions.x / 2.0, 0, terrain.dimensions.y / 2.0);
                mesh.vertices.push_back(position);

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

                mesh.indices.push_back(topLeft);
                mesh.indices.push_back(topRight);
                mesh.indices.push_back(bottomLeft);
                mesh.indices.push_back(bottomRight);
            }
        }

        // Basic validation: ensure indices are inside range
        const unsigned int maxIndex = (unsigned int)(mesh.vertices.size());
        for (size_t k = 0; k < mesh.indices.size(); ++k) {
            if (mesh.indices[k] >= maxIndex) {
                Application::Get()->logger.ThrowRuntimeError("Terrain::Generate: index out of range at index " + std::to_string(k) +
                                                " value: " + std::to_string(mesh.indices[k]) +
                                                " max allowed: " + std::to_string(maxIndex - 1));
            }
        }

        terrain.heightMapTexture = Texture::LoadTextureFromFile("resources/textures/perlin.png", 3, false);
        terrain.perlinNoiseTexture = Texture::LoadTextureFromFile("resources/textures/perlinLarge.png", 3);

        Renderer::CreateMeshBuffers(mesh);
        mesh.material.shader = std::make_shared<Shader>(Application::Get()->renderer.terrainShader);
        mesh.material.albedo = glm::vec3(0.471f, 0.522f, 0.392f); // warm ochre soil tone
        mesh.material.shadowColor = glm::vec3(0.294f, 0.388f, 0.267f); // cooler moss-gray shadow tone
        mesh.material.roughness = 1.0f;
    }
}

void TerrainSystem::Update(entt::registry& registry) {
    Application::Get()->scene.environment.sunDirection = glm::rotateY(Application::Get()->scene.environment.sunDirection, 0.1f * (float)Application::Get()->clock.deltaTime);
}

void TerrainSystem::InsertDrawLogic(Mesh& mesh, entt::entity& entity) {
    if (Application::Get()->scene.registry.try_get<TerrainComponent>(entity)) {
        TerrainComponent terrainComponent = Application::Get()->scene.registry.get<TerrainComponent>(entity);

        Renderer::UploadShaderUniformInt(mesh.material.shader->programId, "heightMap", 0);
        Renderer::UploadShaderUniformFloat(mesh.material.shader->programId, "heightMapStrength", terrainComponent.maxHeight);
        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "cameraPosition", Application::Get()->renderer.camera.position);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrainComponent.heightMapTexture.id);
    }
}
