#include "window.hpp"
#include <quil.h>

Window::Window(unsigned int width, unsigned int height, const char* title) {
    this->width = width;
    this->height = height;
    this->title = title;
}

bool Window::Initialize() {
    if (!glfwInit()) {
        return false;
    }

    glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!glfwWindow) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(glfwWindow);
    quilCreateWindowContext(glfwWindow);
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
