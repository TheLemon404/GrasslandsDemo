#include "clock.hpp"

#include "GLFW/glfw3.h"

void Clock::Tick() {
    time = glfwGetTime();
    deltaTime = time - lastFrameTime;
    lastFrameTime = time;
}

