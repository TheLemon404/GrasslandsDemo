#include <glad/glad.h>
#include "globals.hpp"

Globals globals = {};

int main() {
    globals.window = new Window(1200, 800, "Chess3D");
    if (!globals.window->Initialize()) {
        return -1;
    }

    while (!globals.window->ShouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        globals.window->RefreshAndPoll();
    }

    globals.window->Close();
}
