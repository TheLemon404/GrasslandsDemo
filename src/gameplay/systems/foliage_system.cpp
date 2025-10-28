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
    auto view = registry.view<FoliageComponent, TerrainComponent, InstancedMeshComponent>();
    for (auto& entity : view) {
        TerrainComponent& terrainComponent = registry.get<TerrainComponent>(entity);
        FoliageComponent& foliageComponent = registry.get<FoliageComponent>(entity);
        InstancedMeshComponent& instancedMeshComponent = registry.get<InstancedMeshComponent>(entity);
        instancedMeshComponent.mesh = Renderer::LoadMeshAsset("resources/meshes/grass_blade.obj", "resources/meshes/grass_blade.mtl", true);
        instancedMeshComponent.mesh.material.roughness = 1.0f;
        instancedMeshComponent.mesh.material.shader = std::make_shared<Shader>(Application::Get()->renderer.grassInstancedShader);
        instancedMeshComponent.mesh.cullBackface = false;
        instancedMeshComponent.mesh.castsShadow = false;

        //to compute the placement of the grass in a compute shader
        foliageComponent.foliagePlacementComputeShader = Renderer::CreateComputeShader("resources/shaders/grass_instanced.comp");

        const int lod0NumInstances = foliageComponent.lod0ChunkData.numInstancesPerAxis.x * foliageComponent.lod0ChunkData.numInstancesPerAxis.y;
        const int lod1NumInstances = foliageComponent.lod1ChunkData.numInstancesPerAxis.x * foliageComponent.lod1ChunkData.numInstancesPerAxis.y;

        glGenBuffers(1, &foliageComponent.lod0instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, foliageComponent.lod0instanceSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * lod0NumInstances, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, foliageComponent.lod0instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &foliageComponent.lod1instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, foliageComponent.lod1instanceSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * lod1NumInstances, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, foliageComponent.lod1instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        instancedMeshComponent.transforms.resize(lod0NumInstances);
    }
}

void FoliageSystem::Update(entt::registry& registry) {
    auto view = registry.view<FoliageComponent, TerrainComponent>();
    std::shared_ptr<Application> app = Application::Get();

    for (auto& entity : view) {
        FoliageComponent& foliageComponent = registry.get<FoliageComponent>(entity);
        TerrainComponent& terrainComponent = registry.get<TerrainComponent>(entity);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, foliageComponent.lod0instanceSSBO);
        glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glUseProgram(foliageComponent.foliagePlacementComputeShader.programId);
        Renderer::UploadShaderUniformIVec2(foliageComponent.foliagePlacementComputeShader.programId, "terrainDimensions", terrainComponent.dimensions);
        Renderer::UploadShaderUniformIVec2(foliageComponent.foliagePlacementComputeShader.programId, "foliageChunkDimensions", foliageComponent.lod0ChunkData.chunkDimensions);
        Renderer::UploadShaderUniformIVec2(foliageComponent.foliagePlacementComputeShader.programId, "numInstancesPerAxis", foliageComponent.lod0ChunkData.numInstancesPerAxis);
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

        glDispatchCompute(foliageComponent.lod0ChunkData.numInstancesPerAxis.x / 4, foliageComponent.lod0ChunkData.numInstancesPerAxis.y / 4, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
}

void FoliageSystem::InsertInstancedDrawLogic(Mesh &mesh, entt::entity &entity) {
    if (Application::Get()->scene.registry.try_get<TerrainComponent>(entity) && Application::Get()->scene.registry.try_get<FoliageComponent>(entity)) {
        TerrainComponent terrainComponent = Application::Get()->scene.registry.get<TerrainComponent>(entity);
        FoliageComponent foliageComponent = Application::Get()->scene.registry.get<FoliageComponent>(entity);

        Renderer::UploadShaderUniformVec2(mesh.material.shader->programId, "terrainSpaceUVBounds", terrainComponent.dimensions / 2);

        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "lowerColor",  glm::vec3(0.34f, 0.42f, 0.22f)); // rich olive-green base
        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "upperColor",  glm::vec3(0.78f, 0.86f, 0.46f)); // soft, sunlit yellow-green tip

        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "lowerColor2", glm::vec3(0.28f, 0.38f, 0.26f)); // deeper cool tone in shadows
        Renderer::UploadShaderUniformVec3(mesh.material.shader->programId, "upperColor2", glm::vec3(0.62f, 0.73f, 0.48f)); // muted painterly highlight

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
