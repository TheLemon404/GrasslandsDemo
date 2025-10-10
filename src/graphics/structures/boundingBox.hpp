#pragma once
#include "vec3.hpp"

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;

    bool Contains(glm::vec3 point) {
        return (point.x >= min.x
            && point.y >= min.y
            && point.z >= min.z
            && point.x <= max.x
            && point.y <= max.y
            && point.z <= max.z);
    }
};