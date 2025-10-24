#pragma once

#include <entt/entt.hpp>
#include "components/terrain_component.hpp"
#include "../graphics/renderer.hpp"
#include "components/foliage_component.hpp"
#include "components/instanced_mesh_component.hpp"
#include "components/mesh_component.hpp"
#include "systems/camera_system.hpp"
#include "systems/foliage_system.hpp"
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
    void InsertShadowDrawLogic(Mesh& mesh, entt::entity& entity);
    void InsertDrawLogic(Mesh& mesh, entt::entity& entity);
    void InsertInstancedShadowDrawLogic(Mesh& mesh, entt::entity& entity);
    void InsertInstancedDrawLogic(Mesh& mesh, entt::entity& entity);
};

struct GraphicsDemoScene : Scene {
    GraphicsDemoScene() {
        entt::entity ent = registry.create();
        TransformComponent& transformComponent = registry.emplace<TransformComponent>(ent);
        MeshComponent& mesh = registry.emplace<MeshComponent>(ent);
        mesh.mesh = Renderer::LoadMeshAsset("resources/meshes/box.obj", "resources/meshes/box.mtl", false);
        transformComponent.transform.position.y = 5.0f;

        entt::entity terrain = registry.create();
        //for the terrain mesh and grass foliage
        registry.emplace<TransformComponent>(terrain);
        registry.emplace<MeshComponent>(terrain);
        registry.emplace<TerrainComponent>(terrain);
        registry.emplace<FoliageComponent>(terrain);
        registry.emplace<InstancedMeshComponent>(terrain);

        //add all the needed systems to our scene
        systems.push_back(std::make_unique<TerrainSystem>());
        systems.push_back(std::make_unique<FoliageSystem>());
        systems.push_back(std::make_unique<CameraSystem>());
    }
};
