#include "scene.hpp"

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
