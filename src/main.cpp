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

    globals.scene = GameScene();

    while (!globals.window.ShouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

        globals.renderer.DrawActiveScene();

        globals.window.RefreshAndPoll();
    }

    globals.renderer.CleanUp();
    globals.window.Close();

    return 0;
}
