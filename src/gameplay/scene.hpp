#pragma once

#include <entt/entt.hpp>
#include "components/terrain_mesh_component.hpp"
#include "../graphics/renderer.hpp"
#include "components/instanced_mesh_component.hpp"
#include "components/mesh_component.hpp"
#include "systems/system.hpp"
#include "systems/terrain_system.hpp"
#include "world/environment.hpp"

struct GameObject;

struct Scene {
    Environment environment = {};
    entt::registry registry;
    std::vector<std::shared_ptr<System>> systems;

    void Start();
    void Update();
    void InsertDrawLogic(Mesh& mesh, entt::entity& entity);
};

struct GraphicsDemoScene : Scene {
    GraphicsDemoScene() {
        entt::entity ent = registry.create();
        TransformComponent& transformComponent = registry.emplace<TransformComponent>(ent);
        MeshComponent& mesh = registry.emplace<MeshComponent>(ent);
        mesh.mesh = Renderer::LoadMeshAsset("resources/meshes/box.obj", "resources/meshes/box.mtl", false);
        transformComponent.transform.position.y = 5.0f;

        entt::entity terrain = registry.create();
        registry.emplace<TransformComponent>(terrain);
        registry.emplace<MeshComponent>(terrain);
        registry.emplace<TerrainMeshComponent>(terrain);
        registry.emplace<InstancedMeshComponent>(terrain);

        systems.push_back(std::make_unique<TerrainSystem>());
    }
};