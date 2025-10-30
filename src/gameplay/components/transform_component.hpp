#pragma once
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "fwd.hpp"
#include "vec3.hpp"
#include "../../graphics/structures/transform.hpp"
#include "ext/matrix_transform.hpp"

struct TransformComponent {
    Transform transform;
    entt::entity parent;

    TransformComponent(entt::entity parent = entt::null_t()) : parent(parent) {};
};
