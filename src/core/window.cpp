#include "window.hpp"

#include <iostream>
#include <quil.h>

#include "../application.hpp"
#include "../debug/debug_layer.hpp"
#include "glad/glad.h"

bool Window::Initialize(unsigned int width, unsigned int height, const char* title) {
    this->width = width;
    this->height = height;
    this->title = title;

    if (!glfwInit()) {
        std::cout << "failed to initialize glfw" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, application.settings.msaaSamples);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!glfwWindow) {
        glfwTerminate();
        application.logger.ThrowRuntimeError("failed to create glfw window");
    }

    glfwMakeContextCurrent(glfwWindow);
    glfwSetWindowSizeCallback(glfwWindow, OnResize);
    glfwSetCursorPosCallback(glfwWindow, OnMouseMove);
    glfwSetScrollCallback(glfwWindow, OnMouseScroll);
    quilCreateWindowContext(glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        application.logger.ThrowRuntimeError("failed to load glad");
    }

    application.logger.Log("window successfully initialized");
    return true;
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(glfwWindow);
}

void Window::RefreshAndPoll() {
    mouseProperties.mousePositionLast = mouseProperties.mousePosition;
    mouseProperties.mouseScrollVector = glm::vec2(0, 0);

    glfwPollEvents();
    quilPollCallbacks();
    glfwSwapBuffers(glfwWindow);
}

void Window::Close() {
    glfwTerminate();
}


void Window::OnResize(GLFWwindow *window, int width, int height) {
    application.window.width = width;
    application.window.height = height;
}

void Window::OnMouseMove(GLFWwindow *window, double x, double y) {
    application.window.mouseProperties.mousePositionLast = application.window.mouseProperties.mousePosition;
    application.window.mouseProperties.mousePosition = glm::vec2(x, y);
}

void Window::OnMouseScroll(GLFWwindow *window, double xOffset, double yOffset) {
    application.window.mouseProperties.mouseScrollVector = glm::vec2(xOffset, yOffset);
}
