#include "window.hpp"

#include <iostream>
#include <quil.h>

#include "glad/glad.h"

Window::Window(unsigned int width, unsigned int height, const char* title) {
    this->width = width;
    this->height = height;
    this->title = title;
}

bool Window::Initialize() {
    if (!glfwInit()) {
        std::cout << "failed to initialize glfw" << std::endl;
        return false;
    }

    glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!glfwWindow) {
        std::cout << "failed to create flfw window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(glfwWindow);
    quilCreateWindowContext(glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "failed to load glad" << std::endl;
        return false;
    }

    std::cout << "window successfully initialized" << std::endl;
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
