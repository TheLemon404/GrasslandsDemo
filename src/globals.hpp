#pragma once
#include "core/clock.hpp"
#include "core/io.hpp"
#include "core/logger.hpp"
#include "core/window.hpp"
#include "gameplay/scene.hpp"
#include "graphics/renderer.hpp"

struct Globals {
    Window window;
    Renderer renderer;
    IO io;
    Logger logger;
    Clock clock;

    Scene scene;
};

extern Globals globals;