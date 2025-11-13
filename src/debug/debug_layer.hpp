#pragma once
#include "GLFW/glfw3.h"

struct DebugLayer {
    inline static float frameTimes[60] = {0};

    static void Initialize(GLFWwindow* window);
    static void DrawDebugGUI();
    static void Shutdown();
};
