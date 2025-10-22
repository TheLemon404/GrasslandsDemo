#include <glad/glad.h>
#include <memory>
#include "application.hpp"
#include "quil.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "imgui.h"
#include "debug/debug_layer.hpp"
#include "gtx/rotate_vector.hpp"

int main() {
    std::shared_ptr<Application> app = Application::Get();
    if (!app->Initialize()) {
        return -1;
    }
    quilCreateWindowContext(app->window.glfwWindow);

    DebugLayer::Initialize(app->window.glfwWindow);

    app->scene = GraphicsDemoScene();
    app->scene.Start();

    while (!app->window.ShouldClose()) {
        app->clock.Tick();

        app->scene.Update();
        app->scene.environment.sunDirection = glm::rotateY(app->scene.environment.sunDirection, 0.1f * (float)app->clock.deltaTime);

        if (quilIsKeyPressed(GLFW_KEY_X)) {
            app->renderer.showShadowMapDebug = !app->renderer.showShadowMapDebug;
        }

        app->renderer.DrawActiveScene();

        DebugLayer::DrawDebugGUI();

        quilPollCallbacks();
        app->window.RefreshAndPoll();
    }

    app->Close();

    return 0;
}
