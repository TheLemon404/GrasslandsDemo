#pragma once

#define GLFW_INCLUDE_NONE
#include "vec2.hpp"
#include "GLFW/glfw3.h"

struct MouseProperties {
    glm::vec2 mousePosition;
    glm::vec2 mousePositionLast;
};

class Window {
public:
    unsigned int width, height;
    const char* title;
    GLFWwindow* glfwWindow;

    MouseProperties mouseProperties = {};

    bool Initialize(unsigned int width, unsigned int height, const char* title);
    bool ShouldClose();
    void RefreshAndPoll();
    void Close();

private:
    static void OnResize(GLFWwindow* window, int width, int height);
    static void OnMouseMove(GLFWwindow* window, double x, double y);
};
