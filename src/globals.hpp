#pragma once
#include "core/io.hpp"
#include "core/logger.hpp"
#include "core/window.hpp"
#include "graphics/renderer.hpp"

struct Globals {
    Window* window;
    Renderer* renderer;
    IO* io;
    Logger* logger;
};

extern Globals globals;