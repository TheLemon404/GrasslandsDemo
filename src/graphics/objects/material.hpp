#pragma once
#include "shader.hpp"

struct Material {
    unsigned int shaderProgramId;

    void Set(const char* variable, const void* value);
};
