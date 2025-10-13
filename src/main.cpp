#include <glad/glad.h>
#include "application.hpp"
#include "quil.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "imgui.h"
#include "debug/debug_layer.hpp"
#include "gtx/rotate_vector.hpp"

Application application = Application();

int main() {
    if (!application.Initialize()) {
        return -1;
    }
    quilCreateWindowContext(application.window.glfwWindow);

    DebugLayer::Initialize(application.window.glfwWindow);

    application.scene = GraphicsDemoScene();
    application.scene.Start();

    while (!application.window.ShouldClose()) {
        application.clock.Tick();

        application.scene.Update();
        application.scene.environment.sunDirection = glm::rotateY(application.scene.environment.sunDirection, 0.1f * (float)application.clock.deltaTime);

        application.renderer.camera.position += (application.renderer.camera.position - application.renderer.camera.target) * (-application.window.mouseProperties.mouseScrollVector.y / 20.0f);

        if (quilIsKeyPressed(GLFW_KEY_X)) {
            application.renderer.showShadowMapDebug = !application.renderer.showShadowMapDebug;
        }

        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureMouse) {
            if (quilIsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {
                glm::vec3 cameraForward = glm::normalize(application.renderer.camera.target - application.renderer.camera.position);
                glm::vec3 cameraRight = glm::normalize(glm::cross(application.renderer.camera.up, cameraForward));
                glm::vec2 mouseDelta = application.window.mouseProperties.mousePosition - application.window.mouseProperties.mousePositionLast;
                application.renderer.RotateCameraArount(-mouseDelta.x * application.settings.mouseSensitivityMultiplier, glm::vec3(0, 1, 0), application.renderer.camera.target);
                application.renderer.RotateCameraArount(mouseDelta.y * application.settings.mouseSensitivityMultiplier, cameraRight, application.renderer.camera.target);
            }
        }

        application.renderer.DrawActiveScene();

        DebugLayer::DrawDebugGUI();

        quilPollCallbacks();
        application.window.RefreshAndPoll();
    }

    application.Close();

    return 0;
}
