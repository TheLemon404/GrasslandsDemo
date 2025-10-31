#pragma once

struct Settings {
    float clipDistance = 1000.0f;
    float frustrumClip = 1.1f;
    float mouseMoveSensitivity = 0.1f;
    float mouseLookSensitivity = 0.005f;
    int msaaSamples = 4;
    int shadowFramebufferResolution = 2048;
};
