#include "foliage_system.hpp"
#include "../../application.hpp"
#include "../../graphics/renderer.hpp"
#include "../components/foliage_component.hpp"
#include "../components/instanced_mesh_component.hpp"
#include "glad/glad.h"

void FoliageSystem::Start(entt::registry &registry) {
    auto view = registry.view<InstancedMeshComponent, TerrainComponent, FoliageComponent>();
    for (auto& entity : view) {
        TerrainComponent& terrainComponent = registry.get<TerrainComponent>(entity);
        FoliageComponent& foliageComponent = registry.get<FoliageComponent>(entity);
        InstancedMeshComponent& instancedMeshComponent = registry.get<InstancedMeshComponent>(entity);
        instancedMeshComponent.mesh = Renderer::LoadMeshAsset("resources/meshes/grass_blade.obj", "resources/meshes/grass_blade.mtl", true);
        instancedMeshComponent.mesh.material.roughness = 1.0f;
        instancedMeshComponent.mesh.material.shader = std::make_shared<Shader>(application.renderer.grassInstancedShader);
        instancedMeshComponent.mesh.cullBackface = false;
        instancedMeshComponent.mesh.castsShadow = false;

        //to compute the placement of the grass in a compute shader
        foliageComponent.foliagePlacementComputeShader = Renderer::CreateComputeShader("resources/shaders/grass_instanced.comp");

        //ssbos
        glGenBuffers(1, &instancedMeshComponent.instancedSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instancedMeshComponent.instancedSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * foliageComponent.numInstances, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, instancedMeshComponent.instancedSSBO);

        int sq = sqrt(foliageComponent.numInstances);

        glUseProgram(foliageComponent.foliagePlacementComputeShader.programId);
        Renderer::UploadShaderUniformIVec2(foliageComponent.foliagePlacementComputeShader.programId, "dimensions", foliageComponent.dimensions);
        Renderer::UploadShaderUniformInt(foliageComponent.foliagePlacementComputeShader.programId, "sq", sq);
        glDispatchCompute(sq, sq, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        //this is not good practice, but I need to trick the renderer to draw the correct number of instances
        instancedMeshComponent.transforms.resize(sq * sq);
    }
}

void FoliageSystem::InsertInstancedDrawLogic(Mesh &mesh, entt::entity &entity) {
    if (application.scene.registry.try_get<TerrainComponent>(entity) && application.scene.registry.try_get<FoliageComponent>(entity)) {
        TerrainComponent terrainComponent = application.scene.registry.get<TerrainComponent>(entity);
        InstancedMeshComponent instancedMeshComponent = application.scene.registry.get<InstancedMeshComponent>(entity);
        FoliageComponent foliageComponent = application.scene.registry.get<FoliageComponent>(entity);

        Renderer::UploadShaderUniformVec2(mesh.material.shader->programId, "terrainSpaceUVBounds", terrainComponent.dimensions / 2);
        Renderer::UploadShaderUniformFloat(mesh.material.shader->programId, "time", (float)application.clock.time);
        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "lowerColor", glm::vec3(0.478, 0.702, 0.384));
        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "upperColor", glm::vec3(0.765, 0.941, 0.659));

        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "lowerColor2", glm::vec3(0.706, 0.812, 0.569));
        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "upperColor2", glm::vec3(0.82, 0.941, 0.659));

        Renderer::UploadShaderUniformInt(mesh.material.shader->programId, "perlinTexture", 3);
        Renderer::UploadShaderUniformInt(mesh.material.shader->programId, "heightMap", 0);
        Renderer::UploadShaderUniformFloat(mesh.material.shader->programId, "heightMapStrength", terrainComponent.maxHeight);
        Renderer::UploadShaderUniformFloat(mesh.material.shader->programId, "breezeAmount", foliageComponent.breezeAmount);
        Renderer::UploadShaderUniformFloat(mesh.material.shader->programId, "windAmount", foliageComponent.windAmount);
        Renderer::UploadShaderUniformFloat(mesh.material.shader->programId, "windAngle", foliageComponent.windAngle);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrainComponent.heightMapTexture.id);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, terrainComponent.perlinNoiseTexture.id);
    }
}

