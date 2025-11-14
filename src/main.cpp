#include <glad/glad.h>
#include <memory>
#include "application.hpp"
#include "quil.h"
#include "debug/debug_layer.hpp"

int main() {
    std::shared_ptr<Application> app = Application::Get();
    app->Initialize();

    quilCreateWindowContext(app->window.glfwWindow);

    DebugLayer::Initialize(app->window.glfwWindow);

    app->scene = GraphicsDemoScene();
    app->scene.Start();

    while (!app->window.ShouldClose()) {
        app->clock.Tick();

        app->scene.Update();

        app->renderer.DrawActiveScene();
        DebugLayer::DrawDebugGUI();

        quilPollCallbacks();
        app->window.RefreshAndPoll();
    }

    app->Close();

    return 0;
}
