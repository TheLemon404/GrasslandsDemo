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
        instancedRocks.mesh.material.albedo = glm::vec3(0.58f, 0.63f, 0.70f);
        instancedRocks.mesh.material.shadowColor = glm::vec3(0.42f, 0.48f, 0.54f);
        PlaceOnTerrainRandom(instancedRocks.transforms, terrainComponent.dimensions, glm::ivec2(10), 5.0);


        //tree 1
        entt::entity treeStumps = registry.create();
        TransformComponent& treeStumpsTransformComponent = registry.emplace<TransformComponent>(treeStumps, TransformComponent(terrain));
        InstancedMeshComponent& instancedTreeStumps = registry.emplace<InstancedMeshComponent>(treeStumps);
        instancedTreeStumps.mesh = Renderer::LoadMeshAsset("resources/meshes/tree.obj", "resources/meshes/tree.mtl", true);
        instancedTreeStumps.mesh.material.roughness = 0.8f;
        instancedTreeStumps.mesh.material.albedo = glm::vec3(0.66f, 0.52f, 0.34f);
        instancedTreeStumps.mesh.material.shadowColor = glm::vec3(0.42f, 0.33f, 0.20f);
        PlaceOnTerrainRandom(instancedTreeStumps.transforms, terrainComponent.dimensions, glm::ivec2(30), 3.0f, 3.0f);

        entt::entity trees = registry.create();
        TransformComponent& treesTransformComponent = registry.emplace<TransformComponent>(trees, TransformComponent(terrain));
        InstancedMeshComponent& instancedTrees = registry.emplace<InstancedMeshComponent>(trees);
        instancedTrees.mesh = Renderer::LoadMeshAsset("resources/meshes/tree.obj", "resources/meshes/tree.mtl", true, 1);
        instancedTrees.mesh.cullBackface = false;
        instancedTrees.mesh.material.roughness = 1.0f;
        instancedTrees.mesh.material.applyWind = true;
        instancedTrees.mesh.material.albedo = glm::vec3(0.68f, 0.88f, 0.52f);
        instancedTrees.mesh.material.shadowColor = glm::vec3(0.46f, 0.66f, 0.38f);
        instancedTrees.transforms = instancedTreeStumps.transforms;

        //tree 2
        entt::entity tree2 = registry.create();
        TransformComponent& tree2TransformComponent = registry.emplace<TransformComponent>(tree2, TransformComponent(terrain));
        InstancedMeshComponent& instancedTree2 = registry.emplace<InstancedMeshComponent>(tree2);
        instancedTree2.mesh = Renderer::LoadMeshAsset("resources/meshes/tree2.obj", "resources/meshes/tree2.mtl", true);
        instancedTree2.mesh.material.roughness = 0.8f;
        instancedTree2.mesh.material.applyWind = true;
        instancedTree2.mesh.material.albedo = glm::vec3(0.68f, 0.88f, 0.52f);
        instancedTree2.mesh.material.shadowColor = glm::vec3(0.46f, 0.66f, 0.38f);
        PlaceOnTerrainRandom(instancedTree2.transforms, terrainComponent.dimensions, glm::ivec2(30), 1.0f, 3.0f);

        for(int i = 0; i < instancedTree2.transforms.size(); i++) {
            for(Transform& pt : instancedTreeStumps.transforms) {
                if(instancedTree2.transforms[i].position == pt.position) {
                    instancedTree2.transforms.erase(instancedTree2.transforms.begin() + i);
                }
            }
        }

        entt::entity tree2Stumps = registry.create();
        registry.emplace<TransformComponent>(tree2Stumps, TransformComponent(terrain));
        InstancedMeshComponent& instancedTree2Stumps = registry.emplace<InstancedMeshComponent>(tree2Stumps);
        instancedTree2Stumps.mesh = Renderer::LoadMeshAsset("resources/meshes/tree2.obj", "resources/meshes/tree2.mtl", true, 1);
        instancedTree2Stumps.mesh.cullBackface = false;
        instancedTree2Stumps.mesh.material.roughness = 1.0f;
        instancedTree2Stumps.mesh.material.albedo = glm::vec3(0.66f, 0.52f, 0.34f);
        instancedTree2Stumps.mesh.material.shadowColor = glm::vec3(0.42f, 0.33f, 0.20f);
        instancedTree2Stumps.transforms = instancedTree2.transforms;

        //add all the needed systems to our scene
        systems.push_back(std::make_unique<CameraSystem>());
        systems.push_back(std::make_unique<InstancedMeshSystem>());
        systems.push_back(std::make_unique<TerrainSystem>());
        systems.push_back(std::make_unique<FoliageSystem>());
    }
};
