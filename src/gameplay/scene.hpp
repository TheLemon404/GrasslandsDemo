#pragma once
#include "world/terrain.hpp"
#include "../graphics/renderer.hpp"
#include "world/environment.hpp"

struct Scene {
    Environment environment = {};
    std::vector<Multimesh> meshes;
    std::vector<Instancedmesh> instancedmeshes;
};

struct GameScene : Scene {
    GameScene() {
        meshes.push_back(Renderer::LoadMultimeshAsset("resources/meshes/box.obj", "resources/meshes/box.mtl"));
        meshes.push_back(Renderer::LoadMultimeshAsset("resources/meshes/box.obj", "resources/meshes/box.mtl"));
        meshes[1].transform.scale = glm::vec3(10.0f, 1.0f, 10.0f);
        meshes[1].transform.position.y -= 2.0f;
        Terrain terrain = Terrain();
        terrain.Generate(100, 100);
        meshes.push_back(terrain);
    }
};

struct GraphicsDemoScene : Scene {
    GraphicsDemoScene() {
        meshes.push_back(Renderer::LoadMultimeshAsset("resources/meshes/box.obj", "resources/meshes/box.mtl"));
        meshes.push_back(Renderer::LoadMultimeshAsset("resources/meshes/box.obj", "resources/meshes/box.mtl"));
        meshes[1].transform.scale = glm::vec3(10.0f, 1.0f, 10.0f);
        meshes[1].transform.position.y -= 2.0f;
    }
};