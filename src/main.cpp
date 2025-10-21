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

        app->renderer.camera.position += (app->renderer.camera.position - app->renderer.camera.target) * (-app->window.mouseProperties.mouseScrollVector.y / 20.0f);

        if (quilIsKeyPressed(GLFW_KEY_X)) {
            app->renderer.showShadowMapDebug = !app->renderer.showShadowMapDebug;
        }

        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureMouse) {
            if (quilIsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {
                glm::vec3 cameraForward = glm::normalize(app->renderer.camera.target - app->renderer.camera.position);
                glm::vec3 cameraRight = glm::normalize(glm::cross(app->renderer.camera.up, cameraForward));
                glm::vec2 mouseDelta = app->window.mouseProperties.mousePosition - app->window.mouseProperties.mousePositionLast;
                app->renderer.RotateCameraArount(-mouseDelta.x * app->settings.mouseLookSensitivity, glm::vec3(0, 1, 0), app->renderer.camera.target);
                app->renderer.RotateCameraArount(mouseDelta.y * app->settings.mouseLookSensitivity, cameraRight, app->renderer.camera.target);
            }
            else if (quilIsMouseButtonPressed(GLFW_MOUSE_BUTTON_2)) {
                glm::vec2 mouseDelta = app->window.mouseProperties.mousePosition - app->window.mouseProperties.mousePositionLast;
                glm::vec3 forward = glm::normalize(glm::vec3(app->renderer.camera.view[0][2], 0, app->renderer.camera.view[2][2]));
                glm::vec3 right = glm::normalize(glm::vec3(app->renderer.camera.view[0][0], 0, app->renderer.camera.view[2][0]));
                app->renderer.camera.target -= (forward * mouseDelta.y + right * mouseDelta.x) * app->settings.mouseMoveSensitivity;
                app->renderer.camera.position -= (forward * mouseDelta.y + right * mouseDelta.x) * app->settings.mouseMoveSensitivity;
            }
        }

        app->renderer.DrawActiveScene();

        DebugLayer::DrawDebugGUI();

        quilPollCallbacks();
        app->window.RefreshAndPoll();
    }

    app->Close();

    return 0;
}
