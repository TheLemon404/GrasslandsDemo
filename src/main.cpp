#include <quil.h>
#include "globals.hpp"

Globals globals = {};

int main() {
    globals.window = new Window(1200, 800, "Chess3D");
    if (!globals.window->Initialize()) {
        return -1;
    }

    while (!globals.window->ShouldClose()) {
        globals.window->RefreshAndPoll();
    }

    globals.window->Close();
}
