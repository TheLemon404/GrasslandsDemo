#pragma once
#include <cstddef>
#include <memory>
#define GLFW_INCLUDE_NONE
#include "quil.h"
#include "core/clock.hpp"
#include "core/io.hpp"
#include "core/logger.hpp"
#include "core/window.hpp"
#include "debug/debug_layer.hpp"
#include "gameplay/scene.hpp"
#include "gameplay/structures/settings.hpp"

struct Application {
    Window window;
    Renderer renderer;
    IO io;
    Logger logger;
    Clock clock;
    Scene scene;
    Settings settings;

    inline static std::shared_ptr<Application> instance;
    static std::shared_ptr<Application> Get() {
        if(instance == nullptr) instance = std::make_shared<Application>();

        return instance;
    }

    Application() {
        window = Window();
        renderer = Renderer();
        io = IO();
        logger = Logger();
        clock = Clock();
        settings = Settings();
    }

    bool Initialize() {
        if (!window.Initialize(1200, 800, "GrasslandsDemo")) {
            return false;
        }
        renderer.Initialize();
        return true;
    }

    void Reload() {
        renderer.CleanUp();
        DebugLayer::Shutdown();
        window.Close();

        if (!window.Initialize(window.width, window.height, window.title)) {
            logger.ThrowRuntimeError("failed to close and re-open window");
        }

        renderer.Initialize();
        DebugLayer::Initialize(window.glfwWindow);
        quilCreateWindowContext(window.glfwWindow);

        scene = GraphicsDemoScene();
        scene.Start();
    }

    void Close() {
        renderer.CleanUp();
        DebugLayer::Shutdown();
        window.Close();
    }
};
