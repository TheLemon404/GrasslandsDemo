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

void Scene::InsertDrawLogic(Mesh& mesh) {
    for (std::shared_ptr<System> system : systems) {
        system->InsertDrawLogic(mesh);
    }
}

