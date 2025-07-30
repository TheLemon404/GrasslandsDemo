#pragma once
#include <string>

#include "structures/camera.hpp"
#include "structures/framebuffer.hpp"
#include "structures/multimesh.hpp"
#include "structures/shader.hpp"

class Renderer {
    void DeleteShader(Shader& shader);
    void LoadShaders();

    void UpdateCameraMatrices();
    void UpdateMultimeshMatrices(Multimesh& multimesh);
    void UploadMesh3DMatrices(Mesh& mesh, glm::mat4& transform);
    void UploadMaterialUniforms(Mesh& mesh);

    Camera camera;

    Framebuffer framebuffer;

public:
    Shader meshLitShader;
    Shader prepassShader;

    static Shader CreateShader(std::string vertexShaderLocalPath, std::string fragmentShaderLocalPath);
    static void UploadShaderUniformMat4(unsigned int programId, std::string uniformName, glm::mat4 matrix);
    static void UploadShaderUniformVec4(unsigned int programId, std::string uniformName, glm::vec4 vector);
    static void UploadShaderUniformVec3(unsigned int programId, std::string uniformName, glm::vec3 vector);
    static void UploadShaderUniformVec2(unsigned int programId, std::string uniformName, glm::vec2 vector);
    static void UploadShaderUniformFloat(unsigned int programId, std::string uniformName, float value);
    static void UploadShaderUniformInt(unsigned int programId, std::string uniformName, int value);

    static void CreateMeshBuffers(Mesh& mesh);
    static Multimesh LoadMeshAsset(std::string meshAssetPath);

    static Framebuffer CreateFramebuffer(unsigned int width, unsigned int height);

    void Initialize();
    void DrawActiveScene();
    void CleanUp();
};
