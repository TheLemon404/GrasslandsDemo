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
        meshes.push_back(Renderer::LoadMultimeshAsset("resources/meshes/grid.obj"));
        meshes[0].meshes[0].material.albedo = glm::vec3(1.0f, 0.0f, 0.0f);
    }
};