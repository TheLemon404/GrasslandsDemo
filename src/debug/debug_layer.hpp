#pragma once
#include "GLFW/glfw3.h"

struct DebugLayer {
    static void Initialize(GLFWwindow* window);
    static void DrawDebugGUI();
};
