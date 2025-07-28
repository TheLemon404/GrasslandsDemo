#include "window.hpp"

#include <iostream>
#include <quil.h>

#include "../globals.hpp"
#include "glad/glad.h"

bool Window::Initialize(unsigned int width, unsigned int height, const char* title) {
    this->width = width;
    this->height = height;
    this->title = title;

    if (!glfwInit()) {
        std::cout << "failed to initialize glfw" << std::endl;
        return false;
    }

    glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!glfwWindow) {
        glfwTerminate();
        globals.logger.ThrowRuntimeError("failed to create glfw window");
    }

    glfwMakeContextCurrent(glfwWindow);
    quilCreateWindowContext(glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        globals.logger.ThrowRuntimeError("failed to load glad");
    }

    globals.logger.Log("window successfully initialized");
    return true;
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(glfwWindow);
}

void Window::RefreshAndPoll() {
    glfwPollEvents();
    quilPollCallbacks();
    glfwSwapBuffers(glfwWindow);
}

void Window::Close() {
    glfwTerminate();
}
