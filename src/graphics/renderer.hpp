#pragma once
#include <string>

#include "objects/multimesh.hpp"
#include "objects/shader.hpp"

class Renderer {
    Shader CreateShader(std::string vertexShaderLocalPath, std::string fragmentShaderLocalPath);
    void DeleteShader(Shader& shader);
    void LoadShaders();

    Shader meshLitShader;

public:
    static void CreateMeshBuffers(Mesh& mesh);
    static Multimesh LoadMeshAsset(std::string meshAssetPath);

    void Initialize();
    void DrawActiveScene();
    void CleanUp();
};
