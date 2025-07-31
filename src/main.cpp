#include <glad/glad.h>
#include "globals.hpp"

Globals globals = {};

int main() {
    globals.window = Window();
    globals.renderer = Renderer();
    globals.io = IO();
    globals.logger = Logger();

    if (!globals.window.Initialize(1200, 800, "Chess3D")) {
        return -1;
    }
    globals.renderer.Initialize();

    globals.scene = GameScene();

    while (!globals.window.ShouldClose()) {
        globals.scene.meshes[0].rotation += glm::vec3(0.0f, 0.001f, 0.0f);

        globals.renderer.DrawActiveScene();

        globals.window.RefreshAndPoll();
    }

    globals.renderer.CleanUp();
    globals.window.Close();

    return 0;
}
