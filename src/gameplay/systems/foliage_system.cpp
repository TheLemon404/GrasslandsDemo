#include "foliage_system.hpp"
#include "../../application.hpp"
#include "../../graphics/renderer.hpp"
#include "../components/foliage_component.hpp"
#include "../components/instanced_mesh_component.hpp"
#include "entt/entity/fwd.hpp"
#include "glad/glad.h"
#include <gl/gl.h>
#include <memory>

void FoliageSystem::Start(entt::registry &registry) {
    auto view = registry.view<FoliageComponent, InstancedMeshComponent>();
    for (auto& entity : view) {
        entt::entity terrainEntity = registry.get<TransformComponent>(entity).parent;
        TerrainComponent& terrainComponent = registry.get<TerrainComponent>(terrainEntity);
        FoliageComponent& foliageComponent = registry.get<FoliageComponent>(entity);
        InstancedMeshComponent& instancedMeshComponent = registry.get<InstancedMeshComponent>(entity);
        instancedMeshComponent.mesh = Renderer::LoadMeshAsset(foliageComponent.meshFile + ".obj", true);
        instancedMeshComponent.mesh.material.roughness = 1.0f;
        instancedMeshComponent.mesh.material.shader = std::make_shared<Shader>(Application::Get()->renderer.grassInstancedShader);
        instancedMeshComponent.mesh.cullBackface = false;
        instancedMeshComponent.mesh.castsShadow = false;

        //to compute the placement of the grass in a compute shader
        foliageComponent.foliagePlacementComputeShader = Renderer::CreateComputeShader("resources/shaders/grass_instanced.comp");

        const int numInstances = foliageComponent.chunkData.numInstancesPerAxis.x * foliageComponent.chunkData.numInstancesPerAxis.y;

        glGenBuffers(1, &foliageComponent.instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, foliageComponent.instanceSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * numInstances, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, foliageComponent.instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        instancedMeshComponent.transforms.resize(numInstances);
    }
}

void FoliageSystem::Update(entt::registry& registry) {
    auto view = registry.view<FoliageComponent>();
    std::shared_ptr<Application> app = Application::Get();

    for (auto& entity : view) {
        FoliageComponent& foliageComponent = registry.get<FoliageComponent>(entity);
        entt::entity terrainEntity = registry.get<TransformComponent>(entity).parent;
        TerrainComponent& terrainComponent = registry.get<TerrainComponent>(terrainEntity);

        if(!app->settings.dispatchGrassComputeShader) return;

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, foliageComponent.instanceSSBO);
        glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glUseProgram(foliageComponent.foliagePlacementComputeShader.programId);
        Renderer::UploadShaderUniformIVec2(foliageComponent.foliagePlacementComputeShader.programId, "terrainDimensions", terrainComponent.dimensions);
        Renderer::UploadShaderUniformIVec2(foliageComponent.foliagePlacementComputeShader.programId, "foliageChunkDimensions", foliageComponent.chunkData.chunkDimensions);
        Renderer::UploadShaderUniformIVec2(foliageComponent.foliagePlacementComputeShader.programId, "chunkCenterCutout", foliageComponent.chunkData.chunkCenterCutout);
        Renderer::UploadShaderUniformIVec2(foliageComponent.foliagePlacementComputeShader.programId, "numInstancesPerAxis", foliageComponent.chunkData.numInstancesPerAxis);
        Renderer::UploadShaderUniformVec3(foliageComponent.foliagePlacementComputeShader.programId, "cameraPosition", app->renderer.camera.position);
        Renderer::UploadShaderUniformMat4(foliageComponent.foliagePlacementComputeShader.programId, "cameraProjection", app->renderer.camera.projection);
        Renderer::UploadShaderUniformMat4(foliageComponent.foliagePlacementComputeShader.programId, "cameraView", app->renderer.camera.view);
        Renderer::UploadShaderUniformFloat(foliageComponent.foliagePlacementComputeShader.programId, "clipDistance", app->settings.clipDistance);
        Renderer::UploadShaderUniformFloat(foliageComponent.foliagePlacementComputeShader.programId, "frustrumClip", app->settings.frustrumClip);
        Renderer::UploadShaderUniformInt(foliageComponent.foliagePlacementComputeShader.programId, "heightMap", 0);
        Renderer::UploadShaderUniformFloat(foliageComponent.foliagePlacementComputeShader.programId, "heightMapStrength", terrainComponent.maxHeight);
        Renderer::UploadShaderUniformVec2(foliageComponent.foliagePlacementComputeShader.programId, "terrainSpaceUVBounds", terrainComponent.dimensions / 2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrainComponent.heightMapTexture.id);

        glDispatchCompute(foliageComponent.chunkData.numInstancesPerAxis.x / 16, foliageComponent.chunkData.numInstancesPerAxis.y / 16, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
}

void FoliageSystem::InsertInstancedDrawLogic(Mesh &mesh, entt::entity &entity) {
    if (Application::Get()->scene.registry.try_get<FoliageComponent>(entity)) {
        entt::entity terrainEntity = Application::Get()->scene.registry.get<TransformComponent>(entity).parent;
        TerrainComponent& terrainComponent = Application::Get()->scene.registry.get<TerrainComponent>(terrainEntity);
        FoliageComponent foliageComponent = Application::Get()->scene.registry.get<FoliageComponent>(entity);

        Renderer::UploadShaderUniformVec2(mesh.material.shader->programId, "terrainSpaceUVBounds", terrainComponent.dimensions / 2);

        // Lively but still painterly grass tones
        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "lowerColor",  glm::vec3(0.48f, 0.63f, 0.35f)); // base green, calmer tone
        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "upperColor",  glm::vec3(0.72f, 0.84f, 0.50f)); // lighter, softer tip

        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "lowerColor2", glm::vec3(0.42f, 0.57f, 0.32f)); // shadow variation
        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "upperColor2", glm::vec3(0.68f, 0.80f, 0.55f)); // gentle highlight

        Renderer::UploadShaderUniformInt(mesh.material.shader->programId, "perlinTexture", 3);
        Renderer::UploadShaderUniformFloat(mesh.material.shader->programId, "breezeAmount", foliageComponent.breezeAmount);
        Renderer::UploadShaderUniformFloat(mesh.material.shader->programId, "windAmount", foliageComponent.windAmount);
        Renderer::UploadShaderUniformFloat(mesh.material.shader->programId, "windAngle", foliageComponent.windAngle);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrainComponent.heightMapTexture.id);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, terrainComponent.perlinNoiseTexture.id);
    }
}
