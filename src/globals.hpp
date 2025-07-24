#pragma once
#include "core/io.hpp"
#include "core/window.hpp"
#include "graphics/renderer.hpp"

struct Globals {
    Window* window;
    Renderer* renderer;
    IO* io;
};

extern Globals globals;