#pragma once
#include "system.hpp"

struct TerrainSystem : System {
    void Start(entt::registry& registry) override;
    void Update(entt::registry& registry) override;
    void InsertShadowDrawLogic(Mesh &mesh, entt::entity &entity) override {};
    void InsertDrawLogic(Mesh& mesh, entt::entity& entity) override;
    void InsertInstancedShadowDrawLogic(Mesh &mesh, entt::entity &entity) override {};
    void InsertInstancedDrawLogic(Mesh& mesh, entt::entity& entity) override {};
};
