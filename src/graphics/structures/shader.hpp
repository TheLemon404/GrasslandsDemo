#pragma once

#include "vec3.hpp"

struct Shader {
    const char* vertexShaderSource;
    const char* fragmentShaderSource;

    unsigned int vertexShaderId;
    unsigned int fragmentShaderId;
    unsigned int programId;
};

struct ComputeShader {
    const char* computeShaderSource;

    unsigned int computeShaderId;
    unsigned int programId;
};