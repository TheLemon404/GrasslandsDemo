#include "scene.hpp"
#include <cstdlib>

void Scene::PlaceOnTerrainRandom(std::vector<Transform>& transforms, glm::vec2 terrainDimensions, glm::ivec2 gridDimensions, float placementPercent, float scale, float heightOffset) {
    for(int i = 0; i < gridDimensions.x; i++) {
        for(int j = 0; j < gridDimensions.y; j++) {
            int percent = rand() % 10 + 1;
            if(percent <= placementPercent) {
                float px = i / (float)gridDimensions.x;
                float py = j / (float)gridDimensions.y;

                glm::vec3 pos = glm::vec3(px * terrainDimensions.x, heightOffset, py * terrainDimensions.y) - glm::vec3(terrainDimensions.x / 2.0f, 0.0f, terrainDimensions.y / 2.0f);
                transforms.push_back({
                    .position = pos,
                    .rotation = glm::vec3(0.0f, rand(), 0.0f),
                    .scale = glm::vec3(scale + (rand() / (float)RAND_MAX) * 2.0f)
                });
            }
        }
    }
}

void Scene::Start() {
    for (std::shared_ptr<System> system : systems) {
        system->Start(registry);
    }
}

void Scene::Update() {
    for (std::shared_ptr<System> system : systems) {
        system->Update(registry);
    }
}

void Scene::InsertShadowDrawLogic(Mesh& mesh, entt::entity& entity) {
    for (std::shared_ptr<System> system : systems) {
        system->InsertShadowDrawLogic(mesh, entity);
    }
}

void Scene::InsertDrawLogic(Mesh& mesh, entt::entity& entity) {
    for (std::shared_ptr<System> system : systems) {
        system->InsertDrawLogic(mesh, entity);
    }
}

void Scene::InsertInstancedShadowDrawLogic(Mesh &mesh, entt::entity &entity) {
    for (std::shared_ptr<System> system : systems) {
        system->InsertInstancedShadowDrawLogic(mesh, entity);
    }
}

void Scene::InsertInstancedDrawLogic(Mesh &mesh, entt::entity &entity) {
    for (std::shared_ptr<System> system : systems) {
        system->InsertInstancedDrawLogic(mesh, entity);
    }
}
