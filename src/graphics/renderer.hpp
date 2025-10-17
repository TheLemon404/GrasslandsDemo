#pragma once
#include <string>

#include "tiny_obj_loader.h"
#include "../gameplay/components/transform_component.hpp"
#include "structures/mesh.hpp"
#include "structures/camera.hpp"
#include "structures/framebuffer.hpp"
#include "structures/shader.hpp"

static Mesh fullscreenQuad = {
    .vertices = {
        {0.0f, 0.0f, 0.0f},   // 0
         {1.0f, 0.0f, 0.0f},   // 1
         {1.0f,  1.0f, 0.0f},   // 2
        {0.0f,  1.0f, 0.0f}    // 3
      },
    .uvs = {
        {0.0f, 0.0f},  // 0
        {1.0f, 0.0},  // 1
        {1.0f, 1.0f},  // 2
        {0.0f, 1.0f}  // 3
  },
  .indices = {
        0, 1, 2,
        2, 3, 0
    }
};

class Renderer {
    void DeleteShader(Shader& shader);

    Framebuffer shadowFramebuffer;

public:
    void UpdateCameraMatrices();
    static void UpdateTransform(Transform& transform);
    void UploadMaterialUniforms(Mesh& mesh);
    void UploadTransformationDataUniforms(Mesh& mesh, glm::mat4 transform, glm::mat4 view, glm::mat4 projection, glm::mat4 lightView = glm::identity<glm::mat4>(), glm::mat4 lightProjection = glm::identity<glm::mat4>());

    bool showShadowMapDebug = false;
    bool drawGrass = true;
    bool drawWireframe = false;

    static void CreateMeshBuffers(Mesh& mesh);

    Shader opaqueLitShader;
    Shader opaqueLitInstancedShader;
    Shader postpassShader;
    Shader fullscreenQuadShader;
    Shader terrainShader;
    Shader grassInstancedShader;

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
    static Mesh LoadMeshAsset(std::string meshAssetPath, std::string materialAssetPath, bool instanced);

    static Framebuffer CreateFramebuffer(int width, int height);
    static Framebuffer CreateShadowFramebuffer(int width, int height);

    void Initialize();
    void DrawActiveScene();
    void CleanUp();
};
