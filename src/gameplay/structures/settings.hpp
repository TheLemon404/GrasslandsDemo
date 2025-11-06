#pragma once

struct Settings {
    float clipDistance = 500.0f;
    float frustrumClip = 1.1f;
    float mouseMoveSensitivity = 0.1f;
    float mouseLookSensitivity = 0.005f;
    int msaaSamples = 4;
    int shadowFramebufferResolution = 4098;
    float orthoSize = 375.0f;
    float shadowMapNearPlane = 0.1f;
    float shadowMapFarPlane = 500.0f;
    bool dispatchGrassComputeShader = true;
};
