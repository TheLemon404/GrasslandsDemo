#pragma once

#include <entt/entt.hpp>
#include <memory>
#include "components/terrain_component.hpp"
#include "../graphics/renderer.hpp"
#include "components/foliage_component.hpp"
#include "components/instanced_mesh_component.hpp"
#include "components/mesh_component.hpp"
#include "components/transform_component.hpp"
#include "entt/entity/fwd.hpp"
#include "systems/camera_system.hpp"
#include "systems/foliage_system.hpp"
#include "systems/instanced_mesh_system.hpp"
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

        //for the terrain mesh and grass foliage
        entt::entity terrain = registry.create();
        registry.emplace<TransformComponent>(terrain);
        registry.emplace<MeshComponent>(terrain);
        registry.emplace<TerrainComponent>(terrain);

        entt::entity grassLOD0 = registry.create();
        registry.emplace<TransformComponent>(grassLOD0, TransformComponent(terrain));
        FoliageComponent& foliageComponentLOD0 = registry.emplace<FoliageComponent>(grassLOD0);
        foliageComponentLOD0.meshFile = "resources/meshes/grass_blade";
        registry.emplace<InstancedMeshComponent>(grassLOD0);

        entt::entity rocks = registry.create();
        TransformComponent& rockTransformComponent = registry.emplace<TransformComponent>(rocks, TransformComponent(terrain));
        InstancedMeshComponent& instancedRocks = registry.emplace<InstancedMeshComponent>(rocks);
        instancedRocks.mesh = Renderer::LoadMeshAsset("resources/meshes/rocks.obj", "resources/meshes/rocks.mtl", true);
        instancedRocks.transforms.push_back(
            {
                .position = glm::vec3(0),
                .rotation = glm::vec3(0.0f),
                .scale = glm::vec3(10.0f)
            }
        );

        //add all the needed systems to our scene
        systems.push_back(std::make_unique<CameraSystem>());
        systems.push_back(std::make_unique<InstancedMeshSystem>());
        systems.push_back(std::make_unique<TerrainSystem>());
        systems.push_back(std::make_unique<FoliageSystem>());
    }
};
