#include "terrain_system.hpp"

#include "../../globals.hpp"
#include "../components/mesh_component.hpp"
#include "../components/terrain_component.hpp"
#include "glad/glad.h"

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

        Renderer::CreateMeshBuffers(mesh);
        mesh.material.shaderProgramId = globals.renderer.terrainShader.programId;
        mesh.material.albedo = glm::vec3(0.3f, 1.0f, 0.6f);
        mesh.material.roughness = 1.0f;

        //creating the grass blades
        InstancedMeshComponent& grassInstancedMesh = registry.get<InstancedMeshComponent>(entity);
        grassInstancedMesh.mesh = Renderer::LoadMeshAsset("resources/meshes/grass_blade.obj", "resources/meshes/grass_blade.mtl", true);
        grassInstancedMesh.mesh.material.albedo = glm::vec3(0.3f, 1.0f, 0.6f);
        grassInstancedMesh.mesh.material.shaderProgramId = globals.renderer.grassInstancedShader.programId;
        grassInstancedMesh.mesh.cullBackface = false;
        grassInstancedMesh.mesh.castsShadow = false;

        int sq = sqrt(terrain.grassBlades);
        for (int i = 0; i < sq; i++) {
            for (int j = 0; j < sq; j++) {
                Transform t = {};
                float x = static_cast<float>(i) - sq / 2 + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                float z = static_cast<float>(j) - sq / 2 + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                t.rotation.y = static_cast<float>(rand());
                t.position = {x * (float)terrain.dimensions.x / sq, 0.0f,  z * (float)terrain.dimensions.y / sq};
                Renderer::UpdateTransform(t);
                grassInstancedMesh.transforms.push_back(t);
            }
        }

        //collect the matricies from the transforms objects
        std::vector<glm::mat4> matrices;
        for (Transform& t : grassInstancedMesh.transforms) {
            matrices.push_back(t.matrix);
        }

        glGenBuffers(1, &grassInstancedMesh.instancedVBO);
        glBindBuffer(GL_ARRAY_BUFFER, grassInstancedMesh.instancedVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * grassInstancedMesh.transforms.size(), &matrices[0], GL_STATIC_DRAW);

        std::size_t vec4size = sizeof(glm::vec4);
        glBindVertexArray(grassInstancedMesh.mesh.vao);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (void*)(1 * vec4size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (void*)(2 * vec4size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (void*)(3 * vec4size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
}

void TerrainSystem::Update(entt::registry& registry) {

}

void TerrainSystem::InsertDrawLogic(Mesh& mesh, entt::entity& entity) {
    if (globals.scene.registry.try_get<TerrainComponent>(entity)) {
        TerrainComponent terrain = globals.scene.registry.get<TerrainComponent>(entity);

        Renderer::UploadShaderUniformInt(mesh.material.shaderProgramId, "heightMap", 0);
        Renderer::UploadShaderUniformFloat(mesh.material.shaderProgramId, "heightMapStrength", terrain.maxHeight);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrain.heightMapTexture.id);
    }
}

