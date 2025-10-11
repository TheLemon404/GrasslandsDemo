#pragma once

class Clock {
    double lastFrameTime = 0.0f;

public:
    double time = 0.0f;
    double deltaTime = 0.0f;
    void Tick();
};