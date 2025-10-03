#pragma once
#include <string>

#include "tiny_obj_loader.h"
#include "structures/Instancedmesh.hpp"
#include "structures/camera.hpp"
#include "structures/framebuffer.hpp"
#include "structures/multimesh.hpp"
#include "structures/shader.hpp"

class Renderer {
    void DeleteShader(Shader& shader);
    void LoadShaders();

    void UpdateCameraMatrices();
    static void UpdateTransform(Transform& transform);
    void UploadMesh3DMatrices(Mesh& mesh, glm::mat4& transform, glm::mat4& view, glm::mat4& projection);
    void UploadMaterialUniforms(Mesh& mesh);

    Framebuffer shadowFramebuffer;

public:
    static void CreateMeshBuffers(Mesh& mesh);

    Shader opaqueLitShader;
    Shader opaqueInstancedLitShader;
    Shader shadowPassShader;
    Shader shadowInstancedPassShader;
    Shader postpassShader;

    Camera camera;

    void RotateCameraArount(float angle, glm::vec3 axis, glm::vec3 origin);

    static Shader CreateShader(std::string vertexShaderLocalPath, std::string fragmentShaderLocalPath);
    static void UploadShaderUniformMat4(unsigned int programId, std::string uniformName, glm::mat4 matrix);
    static void UploadShaderUniformVec4(unsigned int programId, std::string uniformName, glm::vec4 vector);
    static void UploadShaderUniformVec3(unsigned int programId, std::string uniformName, glm::vec3 vector);
    static void UploadShaderUniformVec2(unsigned int programId, std::string uniformName, glm::vec2 vector);
    static void UploadShaderUniformFloat(unsigned int programId, std::string uniformName, float value);
    static void UploadShaderUniformInt(unsigned int programId, std::string uniformName, int value);

    static Mesh LoadMeshSubAsset(int subMeshIndex, tinyobj::ObjReader& reader);
    static Multimesh LoadMultimeshAsset(std::string meshAssetPath, std::string materialAssetPath);
    static Instancedmesh LoadInstancedmeshAsset(std::string meshAssetPath, std::string materialAssetPath, std::vector<Transform> transforms);

    static Framebuffer CreateFramebuffer(unsigned int width, unsigned int height);
    static Framebuffer CreateShadowFramebuffer(unsigned int width, unsigned int height);

    void Initialize();
    void DrawActiveScene();
    void CleanUp();
};
