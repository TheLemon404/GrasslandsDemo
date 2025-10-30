#include "../../application.hpp"
#include "instanced_mesh_system.hpp"
#include "../../graphics/renderer.hpp"
#include "../components/instanced_mesh_component.hpp"
#include "glad/glad.h"
#include <gl/gl.h>

void InstancedMeshSystem::Start(entt::registry& registry) {
    auto view = registry.view<InstancedMeshComponent>();
    for (auto& entity : view) {
        InstancedMeshComponent& instancedMeshComponent = registry.get<InstancedMeshComponent>(entity);

        for(Transform& t : instancedMeshComponent.transforms){
            Renderer::UpdateTransform(t);
        }

        glGenBuffers(1, &instancedMeshComponent.instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instancedMeshComponent.instanceSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * instancedMeshComponent.transforms.size(), (const void*)instancedMeshComponent.transforms.data(), GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instancedMeshComponent.instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}

void InstancedMeshSystem::InsertInstancedDrawLogic(Mesh& mesh, entt::entity& entity) {
    InstancedMeshComponent& instancedMeshComponent = Application::Get()->scene.registry.get<InstancedMeshComponent>(entity);
}
