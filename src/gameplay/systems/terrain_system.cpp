#include "terrain_system.hpp"

#include "../../globals.hpp"
#include "../components/mesh_component.hpp"
#include "../components/terrain_mesh_component.hpp"

float generateRandomFloat(float min, float max) {
    // Seed the random number generator (do this once at the start of your program)
    // srand(time(0));
    return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

void TerrainSystem::Start(entt::registry& registry) {
    auto view = registry.view<MeshComponent, TerrainMeshComponent>();
    for (auto& entity : view) {
        TerrainMeshComponent terrain = registry.get<TerrainMeshComponent>(entity);
        Mesh& mesh = registry.get<MeshComponent>(entity).mesh;
        mesh.vertices.clear();
        mesh.indices.clear();
        mesh.uvs.clear();
        mesh.normals.clear();

        // Build vertex positions and uvs
        for (int i = 0; i < terrain.width; ++i) {
            for (int j = 0; j < terrain.length; ++j) {
                float x = static_cast<float>(i) - terrain.width / 2;
                float z = static_cast<float>(j) - terrain.length / 2;
                mesh.vertices.push_back({x * terrain.flatScale, 0.0f, z * terrain.flatScale});

                //this is not actually used in the shader (we calculate the normals again in the fragment shader
                mesh.normals.push_back({0.0f, 1.0f, 0.0f});

                // uv 0..1 over grid
                mesh.uvs.push_back({static_cast<float>(i) / (terrain.width - 1), static_cast<float>(j) / (terrain.length - 1)});
            }
        }

        // Build indices: two triangles per quad
        for (int i = 0; i < terrain.width - 1; ++i) {
            for (int j = 0; j < terrain.length - 1; ++j) {
                unsigned int topLeft     = static_cast<unsigned int>(i * terrain.length + j);
                unsigned int topRight    = topLeft + 1;
                unsigned int bottomLeft  = static_cast<unsigned int>((i + 1) * terrain.length + j);
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
        mesh.castsShadow = false;

        Renderer::CreateMeshBuffers(mesh);
        mesh.material.shaderProgramId = globals.renderer.terrainShader.programId;
        mesh.material.albedo = glm::vec3(0.4f, 0.9f, 0.5f);
        mesh.material.texture = Texture::LoadTextureFromFile("resources/textures/th.png", 3);
        mesh.material.roughness = 1.0f;
    }
}

void TerrainSystem::Update(entt::registry& registry) {

}

void TerrainSystem::InsertDrawLogic(Mesh& mesh) {
    Renderer::UploadShaderUniformInt(mesh.material.shaderProgramId, "heightMap", 0);
    Renderer::UploadShaderUniformFloat(mesh.material.shaderProgramId, "heightMapStrength", 2.0f);
}

