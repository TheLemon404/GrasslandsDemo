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

    protected:
    void PlaceOnTerrainRandom(std::vector<Transform>& transforms, glm::vec2 terrainDimensions, glm::ivec2 gridDimensions, float placementPercent = 3.0f, float scale = 10.0f, float heighOffset = 0.0f);

    public:
    void Start();
    void Update();
    void InsertShadowDrawLogic(Mesh& mesh, entt::entity& entity);
    void InsertDrawLogic(Mesh& mesh, entt::entity& entity);
    void InsertInstancedShadowDrawLogic(Mesh& mesh, entt::entity& entity);
    void InsertInstancedDrawLogic(Mesh& mesh, entt::entity& entity);
};

struct GraphicsDemoScene : Scene {
    GraphicsDemoScene() {
        entt::entity terrain = registry.create();
        registry.emplace<TransformComponent>(terrain);
        registry.emplace<MeshComponent>(terrain);
        TerrainComponent& terrainComponent = registry.emplace<TerrainComponent>(terrain);

        entt::entity grassLOD0 = registry.create();
        registry.emplace<TransformComponent>(grassLOD0, TransformComponent(terrain));
        FoliageComponent& foliageComponentLOD0 = registry.emplace<FoliageComponent>(grassLOD0);
        foliageComponentLOD0.meshFile = "resources/meshes/grass_blade";
        registry.emplace<InstancedMeshComponent>(grassLOD0);

        entt::entity rocks = registry.create();
        TransformComponent& rockTransformComponent = registry.emplace<TransformComponent>(rocks, TransformComponent(terrain));
        InstancedMeshComponent& instancedRocks = registry.emplace<InstancedMeshComponent>(rocks);
        instancedRocks.mesh = Renderer::LoadMeshAsset("resources/meshes/rocks.obj", "resources/meshes/rocks.mtl", true);
        instancedRocks.mesh.material.roughness = 0.5f;
        instancedRocks.mesh.material.albedo = glm::vec3(0.42f);
        instancedRocks.mesh.material.shadowColor = glm::vec3(0.149f);
        PlaceOnTerrainRandom(instancedRocks.transforms, terrainComponent.dimensions, glm::ivec2(10), 5.0);

        entt::entity treeStumps = registry.create();
        TransformComponent& treeStumpsTransformComponent = registry.emplace<TransformComponent>(treeStumps, TransformComponent(terrain));
        InstancedMeshComponent& instancedTreeStumps = registry.emplace<InstancedMeshComponent>(treeStumps);
        instancedTreeStumps.mesh = Renderer::LoadMeshAsset("resources/meshes/tree.obj", "resources/meshes/tree.mtl", true);
        instancedTreeStumps.mesh.material.roughness = 0.8f;
        instancedTreeStumps.mesh.material.albedo = glm::vec3(0.678f, 0.545f, 0.357f);
        instancedTreeStumps.mesh.material.shadowColor = glm::vec3(0.369f, 0.294f, 0.192f);
        PlaceOnTerrainRandom(instancedTreeStumps.transforms, terrainComponent.dimensions, glm::ivec2(50), 4.0f, 3.0f);

        entt::entity trees = registry.create();
        TransformComponent& treesTransformComponent = registry.emplace<TransformComponent>(trees, TransformComponent(terrain));
        InstancedMeshComponent& instancedTrees = registry.emplace<InstancedMeshComponent>(trees);
        instancedTrees.mesh = Renderer::LoadMeshAsset("resources/meshes/tree.obj", "resources/meshes/tree.mtl", true, 1);
        instancedTrees.mesh.material.roughness = 1.0f;
        instancedTrees.mesh.material.albedo = glm::vec3(0.62f, 0.86f, 0.46f);
        instancedTrees.mesh.material.shadowColor = glm::vec3(0.357f, 0.478f, 0.275f);
        instancedTrees.transforms = instancedTreeStumps.transforms;


        //add all the needed systems to our scene
        systems.push_back(std::make_unique<CameraSystem>());
        systems.push_back(std::make_unique<InstancedMeshSystem>());
        systems.push_back(std::make_unique<TerrainSystem>());
        systems.push_back(std::make_unique<FoliageSystem>());
    }
};
