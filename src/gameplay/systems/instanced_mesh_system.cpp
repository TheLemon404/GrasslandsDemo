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
            instancedMeshComponent.matrices.push_back(t.matrix);
        }

        glGenBuffers(1, &instancedMeshComponent.instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instancedMeshComponent.instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instancedMeshComponent.transforms.size(), instancedMeshComponent.matrices.data(), GL_STATIC_DRAW);

        std::size_t vec4size = sizeof(glm::vec4);
        glBindVertexArray(instancedMeshComponent.mesh.vao);

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

void InstancedMeshSystem::InsertInstancedDrawLogic(Mesh& mesh, entt::entity& entity) {
    InstancedMeshComponent& instancedMeshComponent = Application::Get()->scene.registry.get<InstancedMeshComponent>(entity);
}
