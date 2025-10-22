#include "foliage_system.hpp"
#include "../../application.hpp"
#include "../../graphics/renderer.hpp"
#include "../components/foliage_component.hpp"
#include "../components/instanced_mesh_component.hpp"
#include "glad/glad.h"

void FoliageSystem::Start(entt::registry &registry) {
    auto view = registry.view<FoliageComponent>();
    for (auto& entity : view) {
        TerrainComponent& terrainComponent = registry.get<TerrainComponent>(entity);
        FoliageComponent& foliageComponent = registry.get<FoliageComponent>(entity);
        foliageComponent.meshLOD0 = Renderer::LoadMeshAsset("resources/meshes/grass_blade.obj", "resources/meshes/grass_blade.mtl", true);
        foliageComponent.meshLOD0.material.roughness = 1.0f;
        foliageComponent.meshLOD0.material.shader = std::make_shared<Shader>(Application::Get()->renderer.grassInstancedShader);
        foliageComponent.meshLOD0.cullBackface = false;
        foliageComponent.meshLOD0.castsShadow = false;

        //to compute the placement of the grass in a compute shader
        foliageComponent.foliagePlacementComputeShader = Renderer::CreateComputeShader("resources/shaders/grass_instanced.comp");

        const int maxGrassInstances = 2000000;
        glGenBuffers(1, &foliageComponent.instancedSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, foliageComponent.instancedSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * maxGrassInstances, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, foliageComponent.instancedSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        DrawElementsIndirectCommand cmd1 = {};
        cmd1.instanceCount = 0;
        cmd1.count = foliageComponent.meshLOD0.indices.size();

        glGenBuffers(1, &foliageComponent.indirectDrawCommandSSBO);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, foliageComponent.indirectDrawCommandSSBO);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(cmd1), &cmd1, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, foliageComponent.indirectDrawCommandSSBO);

        glUseProgram(foliageComponent.foliagePlacementComputeShader.programId);
        Renderer::UploadShaderUniformIVec2(foliageComponent.foliagePlacementComputeShader.programId, "terrainDimensions", terrainComponent.dimensions);
        Renderer::UploadShaderUniformIVec2(foliageComponent.foliagePlacementComputeShader.programId, "patchDimensions", terrainComponent.dimensions / foliageComponent.numPatches);
        glDispatchCompute(foliageComponent.numPatches.x, foliageComponent.numPatches.y, 1);
        glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    }
}

void FoliageSystem::InsertInstancedDrawLogic(Mesh &mesh, entt::entity &entity) {
    if (Application::Get()->scene.registry.try_get<TerrainComponent>(entity) && Application::Get()->scene.registry.try_get<FoliageComponent>(entity)) {
        TerrainComponent terrainComponent = Application::Get()->scene.registry.get<TerrainComponent>(entity);
        FoliageComponent foliageComponent = Application::Get()->scene.registry.get<FoliageComponent>(entity);

        Renderer::UploadShaderUniformVec2(mesh.material.shader->programId, "terrainSpaceUVBounds", terrainComponent.dimensions / 2);
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
