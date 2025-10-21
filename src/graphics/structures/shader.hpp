#pragma once

#include "vec3.hpp"
#include <iostream>

struct Shader {
    std::string vertexShaderPath;
    std::string tesselationControlShaderPath;
    std::string tesselationEvaluationShaderPath;
    std::string fragmentShaderPath;

    unsigned int vertexShaderId;
    unsigned int tesselationControlShaderId;
    unsigned int tesselationEvaluationShaderId;
    unsigned int fragmentShaderId;
    unsigned int programId;
};

struct ComputeShader {
    std::string computeShaderPath;

    unsigned int computeShaderId;
    unsigned int programId;
};
