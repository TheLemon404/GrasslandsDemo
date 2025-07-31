#pragma once
#include "../graphics/renderer.hpp"
#include "../graphics/structures/multimesh.hpp"
#include "structures/environment.hpp"

struct Scene {
    Environment environment = {};
    std::vector<Multimesh> meshes;
};

struct GameScene : Scene {
    GameScene() {
        meshes.push_back(Renderer::LoadMeshAsset("resources/meshes/box.obj"));
    }
};