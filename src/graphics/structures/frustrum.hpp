#pragma once
#include "boundingBox.hpp"
#include "camera.hpp"
#include "fwd.hpp"
#include "plane.hpp"
#include "gtx/fast_square_root.hpp"

struct Frustrum {
    Plane left = {}, right = {}, bottom = {}, top = {},  near = {}, far = {};

    static Frustrum ComputeFromCamera(Camera camera) {
        glm::mat4 m = camera.projection * camera.view;
        Plane left, right, bottom, top, near, far;

        left.normal = glm::fastNormalize(m[3] + m[0]);
        left.d = -(left.normal.x + camera.position.x + left.normal.y + camera.position.y + left.normal.z + camera.position.z);

        right.normal = glm::fastNormalize(m[3] - m[0]);
        right.d = -(right.normal.x + camera.position.x + right.normal.y + camera.position.y + right.normal.z + camera.position.z);

        bottom.normal = glm::fastNormalize(m[3] + m[1]);
        bottom.d = -(bottom.normal.x + camera.position.x + bottom.normal.y + camera.position.y + bottom.normal.z + camera.position.z);

        top.normal = glm::fastNormalize(m[3] - m[1]);
        top.d = -(top.normal.x + camera.position.x + top.normal.y + camera.position.y + top.normal.z + camera.position.z);

        near.normal = glm::fastNormalize(m[3] + m[2]);
        near.d = -(near.normal.x + camera.position.x + near.normal.y + camera.position.y + near.normal.z + camera.position.z);

        far.normal = glm::fastNormalize(m[3] - m[2]);
        far.d = -(far.normal.x + camera.position.x + far.normal.y + camera.position.y + far.normal.z + camera.position.z);

        return {left, right, bottom, top, near, far};
    }

    bool DoesIntersect(BoundingBox box) {
        glm::vec3 positive, negative;

        positive = glm::vec3((left.normal.x >= 0) ? box.max.x : box.min.x,
            (left.normal.y >= 0) ? box.max.y : box.min.y,
            (left.normal.z >= 0) ? box.max.z : box.min.z);

        negative = glm::vec3((left.normal.x >= 0) ? box.min.x : box.max.x,
            (left.normal.y >= 0) ? box.min.y : box.max.y,
            (left.normal.z >= 0) ? box.min.z : box.max.z);
    }
};
