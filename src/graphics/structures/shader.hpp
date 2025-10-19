#pragma once

#include "vec3.hpp"

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