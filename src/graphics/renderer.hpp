#pragma once

struct Shader {
    const char* vertexShaderSource;
    const char* fragmentShaderSource;

    unsigned int vertexShaderId;
    unsigned int fragmentShaderId;
    unsigned int programId;
};

struct Renderer {
private:
    Shader CreateShader(const char* vertexShaderLocalPath, const char* fragmentShaderLocalPath);
    void LoadShaders();

    void Initialize();
};