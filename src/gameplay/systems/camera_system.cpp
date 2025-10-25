#include "camera_system.hpp"
#include "entt/entity/fwd.hpp"
#include "../../application.hpp"
#include "imgui.h"
#include <memory>

void CameraSystem::Update(entt::registry& registry) {
    std::shared_ptr<Application> app = Application::Get();

    app->renderer.camera.position += (app->renderer.camera.position - app->renderer.camera.target) * (-app->window.mouseProperties.mouseScrollVector.y / 20.0f);

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

    app->renderer.UpdateCameraMatrices();
}
