#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

class Window {
public:
    unsigned int width, height;
    const char* title;
    GLFWwindow* glfwWindow;

    bool Initialize(unsigned int width, unsigned int height, const char* title);
    bool ShouldClose();
    void RefreshAndPoll();
    void Close();

private:
    static void OnResize(GLFWwindow* window, int width, int height);
};
