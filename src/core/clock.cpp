#include "clock.hpp"

#include "GLFW/glfw3.h"

void Clock::Tick() {
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
}

