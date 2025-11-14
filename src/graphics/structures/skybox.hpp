#pragma once

#include "mesh.hpp"

struct Skybox {
    glm::vec3 skyColor = glm::vec3(0.62f, 0.74f, 0.90f);
    glm::vec3 horizonColor = glm::vec3(0.62f, 0.74f, 0.90f);
    glm::vec3 groundColor = glm::vec3(0.78f, 0.80f, 0.73f);

    Mesh cubemapMesh;
    static Skybox LoadSkybox();
};
