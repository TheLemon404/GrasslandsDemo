#include "terrain_system.hpp"

#include "../../globals.hpp"
#include "../components/mesh_component.hpp"
#include "../components/terrain_mesh_component.hpp"

void TerrainSystem::Start(entt::registry& registry) {
    auto view = registry.view<MeshComponent, TerrainMeshComponent>();
    for (auto& entity : view) {
        TerrainMeshComponent terrain = registry.get<TerrainMeshComponent>(entity);
        Mesh& mesh = registry.get<MeshComponent>(entity).mesh;
        mesh.vertices.clear();
        mesh.indices.clear();
        mesh.uvs.clear();
        mesh.normals.clear();

        // reserve to avoid reallocation issues
        const int vertexCount = terrain.width * terrain.height;
        mesh.vertices.reserve(vertexCount * 3);
        mesh.uvs.reserve(vertexCount * 2);
        mesh.normals.reserve(vertexCount * 3);
        mesh.indices.reserve((terrain.width - 1) * (terrain.height - 1) * 6);

        // Build vertex positions and uvs
        for (int i = -terrain.width / 2; i < terrain.width / 2; ++i) {
            for (int j = -terrain.height / 2; j < terrain.height / 2; ++j) {
                float x = static_cast<float>(i);
                float y = 0.0f; // replace with noise for actual terrain
                float z = static_cast<float>(j);
                mesh.vertices.push_back(x * terrain.flatScale);
                mesh.vertices.push_back(y * terrain.heightScale * rand());
                mesh.vertices.push_back(z * terrain.flatScale);

                // uv 0..1 over grid
                mesh.uvs.push_back(static_cast<float>(i) / (terrain.width - 1));
                mesh.uvs.push_back(static_cast<float>(j) / (terrain.height - 1));

                // placeholder normal (will be overwritten if you compute smooth normals)
                mesh.normals.push_back(0.0f);
                mesh.normals.push_back(1.0f);
                mesh.normals.push_back(0.0f);
            }
        }

        // Build indices: two triangles per quad
        for (int i = 0; i < terrain.width - 1; ++i) {
            for (int j = 0; j < terrain.height - 1; ++j) {
                unsigned int topLeft     = static_cast<unsigned int>(i * terrain.width + j);
                unsigned int topRight    = topLeft + 1;
                unsigned int bottomLeft  = static_cast<unsigned int>((i + 1) * terrain.width + j);
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
        const unsigned int maxIndex = (unsigned int)(mesh.vertices.size() / 3);
        for (size_t k = 0; k < mesh.indices.size(); ++k) {
            if (mesh.indices[k] >= maxIndex) {
                // you can log or throw; prefer logging so you can see what's wrong without crashing
                globals.logger.ThrowRuntimeError("Terrain::Generate: index out of range at index " + std::to_string(k) +
                                                " value: " + std::to_string(mesh.indices[k]) +
                                                " max allowed: " + std::to_string(maxIndex - 1));
            }
        }

        mesh.cullBackface = false;
        mesh.shadowCullFace = 1;

        Renderer::CreateMeshBuffers(mesh);
        mesh.material.shaderProgramId = globals.renderer.opaqueLitShader.programId;
        mesh.material.albedo = glm::vec3(0.4f, 0.9f, 0.5f);
        mesh.material.roughness = 1.0f;
    }
}

void TerrainSystem::Update(entt::registry& registry) {
}
