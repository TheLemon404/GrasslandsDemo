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
