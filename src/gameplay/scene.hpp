#pragma once

#include <entt/entt.hpp>
#include "components/terrain_mesh_component.hpp"
#include "../graphics/renderer.hpp"
#include "components/mesh_component.hpp"
#include "systems/system.hpp"
#include "systems/terrain_system.hpp"
#include "world/environment.hpp"

struct GameObject;

struct Scene {
    Environment environment = {};
    std::vector<InstancedMesh> instancedmeshes;
    entt::registry registry;
    std::vector<std::shared_ptr<System>> systems;

    void Start();
    void Update();
};

struct GraphicsDemoScene : Scene {
    GraphicsDemoScene() {
        entt::entity ent = registry.create();
        TransformComponent& transform = registry.emplace<TransformComponent>(ent);
        MeshComponent& mesh = registry.emplace<MeshComponent>(ent);
        mesh.mesh = Renderer::LoadMeshAsset("resources/meshes/box.obj", "resources/meshes/box.mtl");
        transform.position.y = 1.0f;

        entt::entity ent2 = registry.create();
        registry.emplace<TransformComponent>(ent2);
        registry.emplace<MeshComponent>(ent2);
        registry.emplace<TerrainMeshComponent>(ent2);

        systems.push_back(std::make_unique<TerrainSystem>());
    }
};