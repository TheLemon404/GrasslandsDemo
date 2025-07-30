#pragma once
#include "renderbuffer.hpp"
#include "texture.hpp"

struct Framebuffer {
    unsigned int id;

    Texture colorTexture;
    Texture normalTexture;
    Texture positionTexture;
    Renderbuffer depthStencilRenderbuffer;
};
