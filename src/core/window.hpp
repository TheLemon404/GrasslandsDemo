#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

struct Window {
    unsigned int width, height;
    const char* title;
    GLFWwindow* glfwWindow;

    Window(unsigned int width, unsigned int height, const char* title);
    bool Initialize();
    bool ShouldClose();
    void RefreshAndPoll();
    void Close();
};
