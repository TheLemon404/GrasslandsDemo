#pragma once
#include <vector>

#include "fwd.hpp"
#include "mesh.hpp"
#include "../../gameplay/components/transform_component.hpp"

struct InstancedMesh {
    Mesh mesh;
    std::vector<TransformComponent> transforms;
};
