#include "renderer.hpp"

#include <iostream>
#include <ostream>
#include <glad/glad.h>
#include "../globals.hpp"

Shader Renderer::CreateShader(const char *vertexShaderLocalPath, const char *fragmentShaderLocalPath) {
    const char* vertexShaderContent = globals.io->LoadShaderFileContents("");
    const char* fragmentShaderContent = globals.io->LoadShaderFileContents("");

    unsigned int vertexShaderId = 0, fragmentShaderId = 0, programId = 0;

    //vertex shader compilation
    vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &vertexShaderContent, nullptr);
    glCompileShader(vertexShaderId);
    int vertexSuccess;
    char vertexInfoLog[512];
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &vertexSuccess);

    if(!vertexSuccess) {
        glGetShaderInfoLog(vertexShaderId, 512, nullptr, vertexInfoLog);
        std::cout << vertexInfoLog << std::endl;
        return {};
    }

    //fragment shader compilationg
    fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &fragmentShaderContent, nullptr);
    glCompileShader(fragmentShaderId);
    int fragmentSuccess;
    char fragmentInfoLog[512];
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &fragmentSuccess);

    if(!fragmentSuccess) {
        glGetShaderInfoLog(fragmentShaderId, 512, nullptr, fragmentInfoLog);
        std::cout << fragmentInfoLog << std::endl;
        return {};
    }

    programId = glCreateProgram();

    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    int programSuccess;
    char programInfoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &programSuccess);
    if(!programSuccess) {
        glGetProgramInfoLog(programId, 512, nullptr, programInfoLog);
        std::cout << programInfoLog << std::endl;
        return {};
    }

    return {
        vertexShaderContent,
        fragmentShaderContent,
        vertexShaderId,
        fragmentShaderId,
        programId
    };
}

void Renderer::LoadShaders() {

}

void Renderer::Initialize() {
    LoadShaders();
}

