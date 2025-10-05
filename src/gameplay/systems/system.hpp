#pragma once
#include "entt/entity/entity.hpp"
#include "entt/entity/registry.hpp"

struct Mesh;

struct System {
    virtual void Start(entt::registry& registry) = 0;
    virtual void Update(entt::registry& registry) = 0;
    virtual void InsertDrawLogic(Mesh& mesh) = 0;
};
