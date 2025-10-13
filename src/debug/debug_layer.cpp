#include "debug_layer.hpp"
#include <imgui.h>

#include "../globals.hpp"
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
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //master debug window
    ImGui::Begin("Debug");
    ImGui::Checkbox("Show Shadow-Map Debug", &globals.renderer.showShadowMapDebug);
    ImGui::Text("Delta Time: %lf", globals.clock.deltaTime);
    ImGui::Text("FPS: %lf", 1.0 / globals.clock.deltaTime);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
