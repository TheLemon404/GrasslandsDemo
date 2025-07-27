#pragma once

struct Shader {
    const char* vertexShaderSource;
    const char* fragmentShaderSource;

    unsigned int vertexShaderId;
    unsigned int fragmentShaderId;
    unsigned int programId;
};
