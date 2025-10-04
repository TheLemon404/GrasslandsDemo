#include <glad/glad.h>
#include "globals.hpp"
#include "quil.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/rotate_vector.hpp"

Globals globals = {};

int main() {
    globals.window = Window();
    globals.renderer = Renderer();
    globals.io = IO();
    globals.logger = Logger();
    globals.clock = Clock();
    globals.settings = Settings();

    if (!globals.window.Initialize(1200, 800, "Chess3D")) {
        return -1;
    }
    globals.renderer.Initialize();

    globals.scene = GraphicsDemoScene();
    globals.scene.Start();

    while (!globals.window.ShouldClose()) {
        globals.clock.Tick();

        globals.scene.Update();
        globals.scene.environment.sunDirection = glm::rotateY(globals.scene.environment.sunDirection, 0.1f * (float)globals.clock.deltaTime);

        globals.renderer.camera.position += (globals.renderer.camera.position - globals.renderer.camera.target) * (-globals.window.mouseProperties.mouseScrollVector.y / 20.0f);

        if (quilIsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {
            glm::vec3 cameraForward = glm::normalize(globals.renderer.camera.target - globals.renderer.camera.position);
            glm::vec3 cameraRight = glm::normalize(glm::cross(globals.renderer.camera.up, cameraForward));
            glm::vec2 mouseDelta = globals.window.mouseProperties.mousePosition - globals.window.mouseProperties.mousePositionLast;
            globals.renderer.RotateCameraArount(-mouseDelta.x * globals.settings.mouseSensitivityMultiplier, glm::vec3(0, 1, 0), globals.renderer.camera.target);
            globals.renderer.RotateCameraArount(mouseDelta.y * globals.settings.mouseSensitivityMultiplier, cameraRight, globals.renderer.camera.target);
        }

        globals.renderer.DrawActiveScene();

        globals.window.RefreshAndPoll();
    }

    globals.renderer.CleanUp();
    globals.window.Close();

    return 0;
}
