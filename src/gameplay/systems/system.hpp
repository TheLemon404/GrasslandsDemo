#pragma once
#include "entt/entity/entity.hpp"
#include "entt/entity/registry.hpp"

struct System {
    virtual void Start(entt::registry& registry) = 0;
    virtual void Update(entt::registry& registry) = 0;
};
