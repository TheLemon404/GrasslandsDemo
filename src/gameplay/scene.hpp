#pragma once
#include "../graphics/renderer.hpp"
#include "../graphics/structures/multimesh.hpp"
#include "structures/environment.hpp"

struct Scene {
    Environment environment = {};
    std::vector<Multimesh> meshes;
    std::vector<Instancedmesh> instancedmeshes;
};

struct GameScene : Scene {
    GameScene() {
        std::vector<Transform> transforms;
        for (int x = 0; x < 5; x++) {
            for (int y = 0; y < 5; y++) {
                for (int z = 0; z < 5; z++) {
                    glm::vec3 pos = glm::vec3(x, y, z) * 4.0f;
                    transforms.push_back({
                        .position = pos
                    });
                }
            }
        }
        instancedmeshes.push_back(Renderer::LoadInstancedmeshAsset("resources/meshes/box.obj", transforms));
    }
};