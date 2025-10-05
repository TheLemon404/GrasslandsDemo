#pragma once
#include "system.hpp"

struct TerrainSystem : System {
    void Start(entt::registry& registry) override;
    void Update(entt::registry& registry) override;
    void InsertDrawLogic(Mesh& mesh) override;
};
