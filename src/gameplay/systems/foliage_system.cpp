#include "foliage_system.hpp"
#include "../../application.hpp"
#include "../../graphics/renderer.hpp"
#include "../components/foliage_component.hpp"
#include "../components/instanced_mesh_component.hpp"
#include "glad/glad.h"

void FoliageSystem::Start(entt::registry &registry) {
    auto view = registry.view<InstancedMeshComponent, TerrainComponent, FoliageComponent>();
    for (auto& entity : view) {
        TerrainComponent& terrain = registry.get<TerrainComponent>(entity);
        FoliageComponent& foliageComponent = registry.get<FoliageComponent>(entity);
        InstancedMeshComponent& instancedMeshComponent = registry.get<InstancedMeshComponent>(entity);
        instancedMeshComponent.mesh = Renderer::LoadMeshAsset("resources/meshes/bid_billboard.obj", "resources/meshes/bid_billboard.mtl", true);
        instancedMeshComponent.mesh.material.albedo = glm::vec3(0.678f, 0.859f, 0.522f);
        instancedMeshComponent.mesh.material.texture = Texture::LoadTextureFromFile("resources/textures/grass_texture.png", 4, false, true);
        instancedMeshComponent.mesh.material.roughness = 1.0f;
        instancedMeshComponent.mesh.material.shaderProgramId = application.renderer.grassInstancedShader.programId;
        instancedMeshComponent.mesh.cullBackface = false;
        instancedMeshComponent.mesh.castsShadow = false;

        int sq = sqrt(foliageComponent.numInstances);
        for (int i = 0; i < sq; i++) {
            for (int j = 0; j < sq; j++) {
                float x = static_cast<float>(i) - sq / 2 + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                float z = static_cast<float>(j) - sq / 2 + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));

                if (glm::distance(glm::vec2(x, z), glm::vec2(0,0)) <= foliageComponent.cameraCutoffDistance) {
                    Transform t = {};
                    t.rotation.y = static_cast<float>(rand());
                    t.scale = glm::vec3(0.5f, 0.5f, 0.5f);
                    t.position = {x * (float)terrain.dimensions.x / sq, 0.0f,  z * (float)terrain.dimensions.y / sq};
                    Renderer::UpdateTransform(t);
                    instancedMeshComponent.transforms.push_back(t);
                }
            }
        }

        //collect the matricies from the transforms objects
        std::vector<glm::mat4> matrices;
        for (Transform& t : instancedMeshComponent.transforms) {
            matrices.push_back(t.matrix);
        }

        //ssbos
        glCreateBuffers(1, &instancedMeshComponent.instancedSSBO);
        glNamedBufferStorage(instancedMeshComponent.instancedSSBO, sizeof(glm::mat4) * foliageComponent.numInstances, (const void *)matrices.data(), GL_DYNAMIC_STORAGE_BIT);
    }
}

void FoliageSystem::InsertInstancedDrawLogic(Mesh &mesh, entt::entity &entity) {
    if (application.scene.registry.try_get<TerrainComponent>(entity) && application.scene.registry.try_get<FoliageComponent>(entity)) {
        TerrainComponent terrainComponent = application.scene.registry.get<TerrainComponent>(entity);
        InstancedMeshComponent instancedMeshComponent = application.scene.registry.get<InstancedMeshComponent>(entity);
        FoliageComponent foliageComponent = application.scene.registry.get<FoliageComponent>(entity);

        Renderer::UploadShaderUniformVec2(mesh.material.shaderProgramId, "terrainSpaceUVBounds", terrainComponent.dimensions / 2);
        Renderer::UploadShaderUniformFloat(mesh.material.shaderProgramId, "time", (float)application.clock.time);
        Renderer::UploadShaderUniformVec3(mesh.material.shaderProgramId, "lowerColor", glm::vec3(0.678f, 0.859f, 0.522f));
        Renderer::UploadShaderUniformVec3(mesh.material.shaderProgramId, "upperColor", glm::vec3(0.898f, 0.98f, 0.659f));

        Renderer::UploadShaderUniformInt(mesh.material.shaderProgramId, "perlinTexture", 3);
        Renderer::UploadShaderUniformInt(mesh.material.shaderProgramId, "heightMap", 0);
        Renderer::UploadShaderUniformFloat(mesh.material.shaderProgramId, "heightMapStrength", terrainComponent.maxHeight);
        Renderer::UploadShaderUniformFloat(mesh.material.shaderProgramId, "windSwayAmount", foliageComponent.windSwayAmount);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrainComponent.heightMapTexture.id);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, terrainComponent.perlinNoiseTexture.id);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, instancedMeshComponent.instancedSSBO);
    }
}

