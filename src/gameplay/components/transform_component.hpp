#pragma once
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "../../graphics/structures/transform.hpp"

struct TransformComponent {
    Transform transform;
    entt::entity parent;

    TransformComponent(entt::entity parent = entt::null_t()) : parent(parent) {};
};
