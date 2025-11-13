#pragma once

#include "cubemap.hpp"
#include "mesh.hpp"

struct Skybox {
    Mesh cubemapMesh;
    Cubemap cubemapTexture;

    static Skybox LoadSkybox(const char* cubemapTextureAssetPath);
};
