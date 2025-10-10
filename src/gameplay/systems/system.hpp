#pragma once
#include "entt/entity/entity.hpp"
#include "entt/entity/registry.hpp"

struct Mesh;

struct System {
    virtual void Start(entt::registry& registry) = 0;
    virtual void Update(entt::registry& registry) = 0;
    virtual void InsertShadowDrawLogic(Mesh& mesh, entt::entity& entity) = 0;
    virtual void InsertDrawLogic(Mesh& mesh, entt::entity& entity) = 0;
    virtual void InsertInstancedShadowDrawLogic(Mesh& mesh, entt::entity& entity) = 0;
    virtual void InsertInstancedDrawLogic(Mesh& mesh, entt::entity& entity) = 0;
};
