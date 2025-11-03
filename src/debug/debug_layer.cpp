#include "debug_layer.hpp"
#include <imgui.h>
#include <memory>

#include "../application.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

void DebugLayer::Initialize(GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void DebugLayer::DrawDebugGUI() {
    std::shared_ptr<Application> app = Application::Get();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //master debug window
    ImGui::Begin("Debug");
    if (ImGui::CollapsingHeader("Renderer Debug")) {
        ImGui::Checkbox("Show Shadow-Map Debug", &app->renderer.showShadowMapDebug);
        ImGui::Checkbox("Draw Wireframe", &app->renderer.drawWireframe);
        ImGui::Checkbox("Draw Grass", &app->renderer.drawGrass);
        if (ImGui::Button("Reload Shaders")) {
            app->renderer.ReloadShaders();
        }
    }
    if (ImGui::CollapsingHeader("Settings Debug")) {
        ImGui::InputInt("MSAA Samples", &app->settings.msaaSamples);
        ImGui::InputFloat("Frustrum Clip", &app->settings.frustrumClip);
        ImGui::InputFloat("Clip Distance", &app->settings.clipDistance);
        ImGui::InputInt("Shadowmap Framebuffer Resolution", &app->settings.shadowFramebufferResolution);
        ImGui::InputFloat("Shadowmap Camera Ortho View Size", &app->settings.orthoSize);
        ImGui::InputFloat("Shadowmap Near Plane", &app->settings.shadowMapNearPlane);
        ImGui::InputFloat("Shadowmap Far Plane", &app->settings.shadowMapFarPlane);
    }
    int shouldReloadApplication = false;
    if (ImGui::Button("Reload Application")) {
        shouldReloadApplication = true;
    }
    ImGui::Text("Delta Time: %lf", app->clock.deltaTime);
    ImGui::Text("FPS: %lf", 1.0 / app->clock.deltaTime);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (shouldReloadApplication) app->Reload();
}

void DebugLayer::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
