#pragma once
#include "system.hpp"

struct FoliageSystem : System {
private:
    int computeFrameCounter = 0;

public:
    void Start(entt::registry& registry) override;
    void Update(entt::registry& registry) override;
    void InsertShadowDrawLogic(Mesh &mesh, entt::entity &entity) override {};
    void InsertDrawLogic(Mesh& mesh, entt::entity& entity) override {};
    void InsertInstancedShadowDrawLogic(Mesh &mesh, entt::entity &entity) override {};
    void InsertInstancedDrawLogic(Mesh& mesh, entt::entity& entity) override;
};
