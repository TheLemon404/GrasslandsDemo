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
        InstancedMeshComponent& grassInstancedMesh = registry.get<InstancedMeshComponent>(entity);
        grassInstancedMesh.mesh = Renderer::LoadMeshAsset("resources/meshes/bid_billboard.obj", "resources/meshes/bid_billboard.mtl", true);
        grassInstancedMesh.mesh.material.albedo = glm::vec3(0.678f, 0.859f, 0.522f);
        grassInstancedMesh.mesh.material.texture = Texture::LoadTextureFromFile("resources/textures/grass_texture.png", 4, false, true);
        grassInstancedMesh.mesh.material.roughness = 1.0f;
        grassInstancedMesh.mesh.material.shaderProgramId = application.renderer.grassInstancedShader.programId;
        grassInstancedMesh.mesh.cullBackface = false;
        grassInstancedMesh.mesh.castsShadow = false;

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
                    grassInstancedMesh.transforms.push_back(t);
                }
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

void FoliageSystem::Update(entt::registry& registry) {

}

void FoliageSystem::InsertInstancedDrawLogic(Mesh &mesh, entt::entity &entity) {
    if (application.scene.registry.try_get<TerrainComponent>(entity) && application.scene.registry.try_get<FoliageComponent>(entity)) {
        TerrainComponent terrain = application.scene.registry.get<TerrainComponent>(entity);

        Renderer::UploadShaderUniformVec2(mesh.material.shaderProgramId, "terrainSpaceUVBounds", terrain.dimensions / 2);
        Renderer::UploadShaderUniformFloat(mesh.material.shaderProgramId, "time", (float)application.clock.time);
        Renderer::UploadShaderUniformVec3(mesh.material.shaderProgramId, "lowerColor", glm::vec3(0.678f, 0.859f, 0.522f));
        Renderer::UploadShaderUniformVec3(mesh.material.shaderProgramId, "upperColor", glm::vec3(0.898f, 0.98f, 0.659f));

        Renderer::UploadShaderUniformInt(mesh.material.shaderProgramId, "perlinTexture", 3);
        Renderer::UploadShaderUniformInt(mesh.material.shaderProgramId, "heightMap", 0);
        Renderer::UploadShaderUniformFloat(mesh.material.shaderProgramId, "heightMapStrength", terrain.maxHeight);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrain.heightMapTexture.id);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, terrain.perlinNoiseTexture.id);
    }
}

