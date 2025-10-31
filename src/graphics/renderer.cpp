#include "renderer.hpp"

#include <algorithm>

#include "tiny_obj_loader.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <regex>
#include <glad/glad.h>
#include "../application.hpp"
#include "glm.hpp"
#include "../gameplay/components/instanced_mesh_component.hpp"
#include "../gameplay/components/mesh_component.hpp"
#include "../gameplay/components/transform_component.hpp"
#include "ext/matrix_clip_space.hpp"
#include "ext/matrix_transform.hpp"
#include "gtc/type_ptr.inl"
#include "utils/parser.hpp"

void Renderer::CreateMeshBuffers(Mesh& mesh) {
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    if (!mesh.indices.empty()) {
        glGenBuffers(1, &mesh.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    }

    if (!mesh.vertices.empty()) {
        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.GetVerticesRaw().size() * sizeof(float), mesh.GetVerticesRaw().data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    if (!mesh.normals.empty()) {
        glGenBuffers(1, &mesh.nbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.nbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.GetNormalsRaw().size() * sizeof(float), mesh.GetNormalsRaw().data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
    }
    else {
        glDisableVertexAttribArray(1);
    }

    if (!mesh.uvs.empty()) {
        glGenBuffers(1, &mesh.uvbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.uvbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.GetUvsRaw().size() * sizeof(float), mesh.GetUvsRaw().data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);
    }
    else {
        glDisableVertexAttribArray(2);
    }
}

Mesh Renderer::LoadMeshSubAsset(int subMeshIndex, tinyobj::ObjReader& reader) {
    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    Mesh result = {};

    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[subMeshIndex].mesh.num_face_vertices.size(); f++) {
        size_t fv = size_t(shapes[subMeshIndex].mesh.num_face_vertices[f]);

        // Loop over vertices in the face.
        for (size_t v = 0; v < fv; v++) {
            // access to vertex
            tinyobj::index_t idx = shapes[subMeshIndex].mesh.indices[index_offset + v];

            tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
            tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
            tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

            result.vertices.push_back({vx, vy, vz});

            // Check if `normal_index` is zero or positive. negative = no normal data
            if (idx.normal_index >= 0) {
                tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                result.normals.push_back({nx, ny, nz});
            }

            // Check if `texcoord_index` is zero or positive. negative = no texcoord data
            if (idx.texcoord_index >= 0) {
                tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                result.uvs.push_back({tx, ty});
            }

            // Optional: vertex colors
            // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
            // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
            // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

            result.indices.push_back(result.vertices.size() - 1);
        }
        index_offset += fv;

        // per-face material
        int matId = shapes[subMeshIndex].mesh.material_ids[f];
    }

    return result;
}

Mesh Renderer::LoadMeshAsset(std::string meshAssetPath, std::string materialAssetPath, bool instanced) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = materialAssetPath.c_str();

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(meshAssetPath.c_str(), reader_config)) {
        if (!reader.Error().empty()) {
            Application::Get()->logger.ThrowRuntimeError("TinyObjReader: " + reader.Error());
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        Application::Get()->logger.Log("TinyObjReader: " + reader.Warning());
    }

    auto& shapes = reader.GetShapes();

    std::vector<Mesh> meshes = {};
    meshes.resize(shapes.size());

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        meshes[s] = LoadMeshSubAsset(s, reader);
    }

    for (Mesh& mesh : meshes) {
        CreateMeshBuffers(mesh);
        if (instanced) {
            mesh.material.shader = std::make_shared<Shader>(Application::Get()->renderer.opaqueLitInstancedShader);
        }
        else {
            mesh.material.shader = std::make_shared<Shader>(Application::Get()->renderer.opaqueLitShader);
        }
    }

    Application::Get()->logger.Log("successfully loaded obj file: " + meshAssetPath);
    return meshes[0];
}

void Renderer::RotateCameraArount(float angle, glm::vec3 axis, glm::vec3 origin) {
    // Create the transformation matrix
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -origin);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
    glm::mat4 inverseTranslationMatrix = glm::translate(glm::mat4(1.0f), origin);
    glm::mat4 finalTransformation = inverseTranslationMatrix * rotationMatrix * translationMatrix;

    // Apply the transformation to a point
    glm::vec4 originalPoint = glm::vec4(camera.position, 1.0f); // Assuming the object's origin
    camera.position = finalTransformation * originalPoint;
}

Shader Renderer::CreateShader(std::string vertexShaderLocalPath, std::string fragmentShaderLocalPath, std::string tesselationControlShaderLocalPath, std::string tesselationEvaluationShaderLocalPath) {
    SE::Util::Parser vertexParser(vertexShaderLocalPath.c_str());
    SE::Util::Parser fragmentParser(fragmentShaderLocalPath.c_str());

    unsigned int vertexShaderId = 0, tesselationControlShaderId = 0, tesselationEvaluationShaderId = 0, fragmentShaderId = 0, programId = 0;

    //vertex shader compilation
    vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexCodeTemp = vertexParser.content.c_str();
    glShaderSource(vertexShaderId, 1, &vertexCodeTemp, nullptr);
    glCompileShader(vertexShaderId);
    int vertexSuccess;
    char vertexInfoLog[512];
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &vertexSuccess);

    if(!vertexSuccess) {
        glGetShaderInfoLog(vertexShaderId, 512, nullptr, vertexInfoLog);
        Application::Get()->logger.ThrowRuntimeError("Vertex Shader error at: " + vertexShaderLocalPath + " : " + vertexInfoLog);
    }

    Application::Get()->logger.Log("successfully compiled vertex shader: " + vertexShaderLocalPath);

    //fragment shader compilationg
    fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentCodeTemp = fragmentParser.content.c_str();
    glShaderSource(fragmentShaderId, 1, &fragmentCodeTemp, nullptr);
    glCompileShader(fragmentShaderId);
    int fragmentSuccess;
    char fragmentInfoLog[512];
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &fragmentSuccess);

    if(!fragmentSuccess) {
        glGetShaderInfoLog(fragmentShaderId, 512, nullptr, fragmentInfoLog);
        Application::Get()->logger.ThrowRuntimeError("Fragment Shader error at: " + fragmentShaderLocalPath + " : " + fragmentInfoLog);
    }

    Application::Get()->logger.Log("successfully compiled fragment shader: " + fragmentShaderLocalPath);

    programId = glCreateProgram();

    //optional tesselation shader compilation
    if (!tesselationControlShaderLocalPath.empty() && !tesselationEvaluationShaderLocalPath.empty()) {
        SE::Util::Parser tesselationControlParser(tesselationControlShaderLocalPath.c_str());

        //tesselation control shader
        tesselationControlShaderId = glCreateShader(GL_TESS_CONTROL_SHADER);
        const char* tescCodeTemp = tesselationControlParser.content.c_str();
        glShaderSource(tesselationControlShaderId, 1, &tescCodeTemp, nullptr);
        glCompileShader(tesselationControlShaderId);
        int tescSuccess;
        char tescInfoLog[512];
        glGetShaderiv(tesselationControlShaderId, GL_COMPILE_STATUS, &tescSuccess);

        if(!tescSuccess) {
            glGetShaderInfoLog(tesselationControlShaderId, 512, nullptr, tescInfoLog);
            Application::Get()->logger.ThrowRuntimeError("Tesselation Control Shader error at: " + tesselationControlShaderLocalPath + " : " + tescInfoLog);
        }

        Application::Get()->logger.Log("successfully compiled tesselation control shader: " + tesselationControlShaderLocalPath);

        //tesselation evaluation shader
        SE::Util::Parser tesselationEvaluationParser(tesselationEvaluationShaderLocalPath.c_str());

        tesselationEvaluationShaderId = glCreateShader(GL_TESS_EVALUATION_SHADER);
        const char* teseCodeTemp = tesselationEvaluationParser.content.c_str();
        glShaderSource(tesselationEvaluationShaderId, 1, &teseCodeTemp, nullptr);
        glCompileShader(tesselationEvaluationShaderId);
        int teseSuccess;
        char teseInfoLog[512];
        glGetShaderiv(tesselationEvaluationShaderId, GL_COMPILE_STATUS, &teseSuccess);

        if(!teseSuccess) {
            glGetShaderInfoLog(tesselationEvaluationShaderId, 512, nullptr, teseInfoLog);
            Application::Get()->logger.ThrowRuntimeError("Tesselation Evaluation Shader error at: " + tesselationEvaluationShaderLocalPath + " : " + teseInfoLog);
        }

        Application::Get()->logger.Log("successfully compiled tesselation evaluation shader: " + tesselationEvaluationShaderLocalPath);

        glAttachShader(programId, tesselationControlShaderId);
        glAttachShader(programId, tesselationEvaluationShaderId);
    }

    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    glDeleteShader(tesselationControlShaderId);
    glDeleteShader(tesselationEvaluationShaderId);

    int programSuccess;
    char programInfoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &programSuccess);
    if(!programSuccess) {
        glGetProgramInfoLog(programId, 512, nullptr, programInfoLog);
        Application::Get()->logger.ThrowRuntimeError(programInfoLog);
    }

    return {
        vertexShaderLocalPath,
        tesselationControlShaderLocalPath,
        tesselationEvaluationShaderLocalPath,
        fragmentShaderLocalPath,
        vertexShaderId,
        tesselationControlShaderId,
        tesselationEvaluationShaderId,
        fragmentShaderId,
        programId
    };
}

ComputeShader Renderer::CreateComputeShader(std::string computeShaderLocalPath) {
    SE::Util::Parser computeParser(computeShaderLocalPath.c_str());

    unsigned int computeShaderId = 0, programId = 0;

    //vertex shader compilation
    computeShaderId = glCreateShader(GL_COMPUTE_SHADER);
    const char* vertexCodeTemp = computeParser.content.c_str();
    glShaderSource(computeShaderId, 1, &vertexCodeTemp, nullptr);
    glCompileShader(computeShaderId);
    int computeSuccess;
    char computeInfoLog[512];
    glGetShaderiv(computeShaderId, GL_COMPILE_STATUS, &computeSuccess);

    if(!computeSuccess) {
        glGetShaderInfoLog(computeShaderId, 512, nullptr, computeInfoLog);
        Application::Get()->logger.ThrowRuntimeError(computeInfoLog);
    }

    programId = glCreateProgram();

    glAttachShader(programId, computeShaderId);
    glLinkProgram(programId);

    glDeleteShader(computeShaderId);

    int programSuccess;
    char programInfoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &programSuccess);
    if(!programSuccess) {
        glGetProgramInfoLog(programId, 512, nullptr, programInfoLog);
        Application::Get()->logger.ThrowRuntimeError("Compute Shader error at: " + computeShaderLocalPath + " : " + programInfoLog);
    }

    Application::Get()->logger.Log("successfully compiled compute shader: " + std::string(computeShaderLocalPath));

    return {
        computeShaderLocalPath,
        computeShaderId,
        programId
    };
}

void Renderer::ReloadShaders() {
    ReloadShader(opaqueLitShader);
    ReloadShader(opaqueLitInstancedShader);
    ReloadShader(postpassShader);
    ReloadShader(fullscreenQuadShader);
    ReloadShader(terrainShader);
    ReloadShader(grassInstancedShader);
}

void Renderer::ReloadComputeShader(ComputeShader& computeShader) {
    glDeleteProgram(computeShader.programId);
    computeShader.programId = CreateComputeShader(computeShader.computeShaderPath).programId;
}

void Renderer::ReloadShader(Shader& shader) {
    glDeleteProgram(shader.programId);
    shader.programId = CreateShader(
        shader.vertexShaderPath,
        shader.fragmentShaderPath,
        shader.tesselationControlShaderPath.empty() ? "" : shader.tesselationControlShaderPath,
        shader.tesselationEvaluationShaderPath.empty() ? "" : shader.tesselationEvaluationShaderPath
        ).programId;
}

void Renderer::UploadShaderUniformMat4(unsigned int programId, std::string uniformName, glm::mat4 matrix) {
    int location = glGetUniformLocation(programId, uniformName.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Renderer::UploadShaderUniformVec4(unsigned int programId, std::string uniformName, glm::vec4 vector) {
    int location = glGetUniformLocation(programId, uniformName.c_str());
    glUniform4fv(location, 1, glm::value_ptr(vector));
}

void Renderer::UploadShaderUniformVec3(unsigned int programId, std::string uniformName, glm::vec3 vector) {
    int location = glGetUniformLocation(programId, uniformName.c_str());
    glUniform3fv(location, 1, glm::value_ptr(vector));
}

void Renderer::UploadShaderUniformVec2(unsigned int programId, std::string uniformName, glm::vec2 vector) {
    int location = glGetUniformLocation(programId, uniformName.c_str());
    glUniform2fv(location, 1, glm::value_ptr(vector));
}

void Renderer::UploadShaderUniformIVec2(unsigned int programId, std::string uniformName, glm::ivec2 vector) {
    int location = glGetUniformLocation(programId, uniformName.c_str());
    glUniform2iv(location, 1, glm::value_ptr(vector));
}

void Renderer::UploadShaderUniformFloat(unsigned int programId, std::string uniformName, float value) {
    int location = glGetUniformLocation(programId, uniformName.c_str());
    glUniform1f(location, value);
}

void Renderer::UploadShaderUniformInt(unsigned int programId, std::string uniformName, int value) {
    int location = glGetUniformLocation(programId, uniformName.c_str());
    glUniform1i(location, value);
}

void Renderer::DeleteShader(Shader& shader) {
    glDeleteProgram(shader.programId);
}

Framebuffer Renderer::CreateFramebuffer(int width, int height) {
    Framebuffer framebuffer = {
        .colorTexture = {
            .width = width,
            .height = height},
        .normalTexture = {
            .width = width,
            .height = height
        },
        .positionTexture = {
            .width = width,
            .height = height
        },
        .materialTexture = {
            .width = width,
            .height = height
        },
        .depthStencilRenderbuffer = {
            .width = width,
            .height = height
        }
    };

    glGenFramebuffers(1, &framebuffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);

    glGenTextures(1, &framebuffer.colorTexture.id);
    glBindTexture(GL_TEXTURE_2D, framebuffer.colorTexture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.colorTexture.id, 0);

    glGenTextures(1, &framebuffer.normalTexture.id);
    glBindTexture(GL_TEXTURE_2D, framebuffer.normalTexture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, framebuffer.normalTexture.id, 0);

    glGenTextures(1, &framebuffer.positionTexture.id);
    glBindTexture(GL_TEXTURE_2D, framebuffer.positionTexture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, framebuffer.positionTexture.id, 0);

    glGenTextures(1, &framebuffer.materialTexture.id);
    glBindTexture(GL_TEXTURE_2D, framebuffer.materialTexture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, framebuffer.materialTexture.id, 0);

    glGenRenderbuffers(1, &framebuffer.depthStencilRenderbuffer.id);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depthStencilRenderbuffer.id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depthStencilRenderbuffer.id);

    const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Application::Get()->logger.ThrowRuntimeError("failed to create framebuffer");
    }

    return framebuffer;
}

Framebuffer Renderer::CreateShadowFramebuffer(int width, int height) {
    Framebuffer framebuffer = {
        .depthTexture = {
            .width = width,
            .height = height
        }
    };

    glGenFramebuffers(1, &framebuffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);

    glGenTextures(1, &framebuffer.depthTexture.id);
    glBindTexture(GL_TEXTURE_2D, framebuffer.depthTexture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer.depthTexture.id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Application::Get()->logger.ThrowRuntimeError("failed to create framebuffer");
    }

    return framebuffer;
}

void Renderer::UpdateCameraMatrices() {
    camera.projection = glm::identity<glm::mat4>();
    camera.projection = glm::perspective<float>(camera.fov, (float)Application::Get()->window.width / (float)Application::Get()->window.height, camera.near, camera.far);

    camera.view = glm::identity<glm::mat4>();
    camera.view = glm::lookAt(camera.position, camera.target, camera.up);
}

void Renderer::UpdateTransform(Transform& transform) {
    transform.matrix = glm::identity<glm::mat4>();
    transform.matrix = glm::translate(transform.matrix, transform.position);
    transform.matrix = glm::rotate(transform.matrix, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    transform.matrix = glm::rotate(transform.matrix, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    transform.matrix = glm::rotate(transform.matrix, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    transform.matrix = glm::scale(transform.matrix, transform.scale);
}

void Renderer::UploadMaterialUniforms(Mesh &mesh) {
    UploadShaderUniformVec3(mesh.material.shader->programId, "material.albedo", mesh.material.albedo);
    UploadShaderUniformVec3(mesh.material.shader->programId, "material.shadowColor", mesh.material.shadowColor);
    UploadShaderUniformFloat(mesh.material.shader->programId, "material.roughness", mesh.material.roughness);
    if (mesh.material.texture.width == 0 && mesh.material.texture.height == 0) {
        UploadShaderUniformInt(mesh.material.shader->programId, "material.hasBaseTexture", 0);
    }
    else {
        UploadShaderUniformInt(mesh.material.shader->programId, "material.baseTexture", 2);
        UploadShaderUniformInt(mesh.material.shader->programId, "material.hasBaseTexture", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mesh.material.texture.id);
    }
}

void Renderer::UploadTransformationDataUniforms(Mesh &mesh, glm::mat4 transform, glm::mat4 view, glm::mat4 projection, glm::mat4 lightView, glm::mat4 lightProjection) {
    UploadShaderUniformMat4(mesh.material.shader->programId, "transformationData.transform", transform);
    UploadShaderUniformMat4(mesh.material.shader->programId, "transformationData.view", view);
    UploadShaderUniformMat4(mesh.material.shader->programId, "transformationData.projection", projection);
    UploadShaderUniformMat4(mesh.material.shader->programId, "transformationData.lightView", lightView);
    UploadShaderUniformMat4(mesh.material.shader->programId, "transformationData.lightProjection", lightProjection);
}

void Renderer::Initialize() {
    std::shared_ptr<Application> app = Application::Get();
    opaqueLitInstancedShader = CreateShader("resources/shaders/opaque_instanced_lit.vert", "resources/shaders/opaque_instanced_lit.frag");
    opaqueLitShader = CreateShader("resources/shaders/opaque_lit.vert", "resources/shaders/opaque_lit.frag");
    postpassShader = CreateShader("resources/shaders/postpass.vert", "resources/shaders/postpass.frag");
    fullscreenQuadShader = CreateShader("resources/shaders/fullscreen_quad.vert", "resources/shaders/fullscreen_quad.frag");
    terrainShader = CreateShader("resources/shaders/terrain.vert", "resources/shaders/terrain.frag", "resources/shaders/terrain.tesc", "resources/shaders/terrain.tese");
    grassInstancedShader = CreateShader("resources/shaders/grass_instanced.vert", "resources/shaders/grass_instanced.frag");

    shadowFramebuffer = CreateShadowFramebuffer(app->settings.shadowFramebufferResolution, Application::Get()->settings.shadowFramebufferResolution);

    UpdateCameraMatrices();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);

    glPatchParameteri(GL_PATCH_VERTICES, 4);

    //create default objects (fullscreen quad etc...)
    CreateMeshBuffers(fullscreenQuad);
    fullscreenQuad.material.texture.id = shadowFramebuffer.depthTexture.id;
    fullscreenQuad.material.shader = std::make_shared<Shader>(app->renderer.fullscreenQuadShader);
}

void Renderer::DrawActiveScene() {
    std::shared_ptr<Application> app = Application::Get();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //shadow pass
    glCullFace(GL_BACK);
    glViewport(0, 0, app->settings.shadowFramebufferResolution, app->settings.shadowFramebufferResolution);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer.id);
    glClear(GL_DEPTH_BUFFER_BIT);

    //"dynamic" shadow pass (this desperately needs to be re-done).
    float orthoSize = 50.0f;
    float near_plane = 0.01f;
    float far_plane  = 20.0f;
    glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);
    // place the light at some distance in the light direction behind the scene center
    glm::vec3 sceneCenter = glm::vec3(0.0f, 0.0f, 0.0f); // choose logical scene center or camera target
    glm::vec3 lightDir = glm::normalize(Application::Get()->scene.environment.sunDirection); // direction the sun shines
    float lightDistance = 5.0f; // tune this so the light is sufficiently far out
    glm::vec3 lightPos = sceneCenter - lightDir * lightDistance; // position the light "behind" the scene
    glm::vec3 lightTarget = sceneCenter;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, up);

    //regular mesh shadow pass
    auto view = app->scene.registry.view<MeshComponent, TransformComponent>();
    for (auto entity : view) {
        TransformComponent transformComponent = app->scene.registry.get<TransformComponent>(entity);
        UpdateTransform(transformComponent.transform);
        Mesh mesh = app->scene.registry.get<MeshComponent>(entity).mesh;

        if (mesh.castsShadow) {
            if (mesh.shadowCullFace == 0) {
                glCullFace(GL_BACK);
            }
            else {
                glCullFace(GL_FRONT);
            }

            if (mesh.vao == 0) {
                app->logger.ThrowRuntimeError("MAJOR ERROR: attempting to draw a mesh that has no VAO (you probably forgot to call Renderer::CreateMeshBuffers() somwhere");
            }
            glBindVertexArray(mesh.vao);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glUseProgram(mesh.material.shader->programId);

            UploadTransformationDataUniforms(mesh, transformComponent.transform.matrix, lightView, lightProjection);

            app->scene.InsertShadowDrawLogic(mesh, entity);

            if (!mesh.material.shader->tesselationControlShaderPath.empty() && !mesh.material.shader->tesselationEvaluationShaderPath.empty()) {
                if (!mesh.indices.empty()) {
                    glDrawElements(GL_PATCHES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
                }
                else {
                    glDrawArrays(GL_PATCHES, 0, mesh.vertices.size() / 3);
                }
            }
            else {
                if (!mesh.indices.empty()) {
                    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
                }
                else {
                    glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size() / 3);
                }
            }

            glUseProgram(0);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            glBindVertexArray(0);
        }
    }

    //instanced mesh shadow pass
    auto view2 = app->scene.registry.view<InstancedMeshComponent>();
    for (auto entity : view2) {
        InstancedMeshComponent& instancedMeshComponent = view2.get<InstancedMeshComponent>(entity);
        Mesh mesh = instancedMeshComponent.mesh;

        if (mesh.castsShadow) {
            if (mesh.shadowCullFace == 0) {
                glCullFace(GL_BACK);
            }
            else {
                glCullFace(GL_FRONT);
            }

            if (mesh.vao == 0) {
                app->logger.ThrowRuntimeError("MAJOR ERROR: attempting to draw a mesh that has no VAO (you probably forgot to call Renderer::CreateMeshBuffers() somwhere");
            }
            glBindVertexArray(mesh.vao);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glUseProgram(mesh.material.shader->programId);

            UploadTransformationDataUniforms(mesh, glm::identity<glm::mat4>(), lightView, lightProjection);

            app->scene.InsertInstancedShadowDrawLogic(mesh, entity);

            if (!mesh.indices.empty()) {
                glDrawElementsInstanced(GL_TRIANGLES,  mesh.indices.size(), GL_UNSIGNED_INT, 0, instancedMeshComponent.transforms.size());
            }
            else {
                glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.vertices.size() / 3, instancedMeshComponent.transforms.size());
            }

            glUseProgram(0);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            glDisableVertexAttribArray(3);
            glBindVertexArray(0);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //forward pass
    glCullFace(GL_FRONT);
    glViewport(0, 0, app->window.width, app->window.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(app->scene.environment.clearColor.r, app->scene.environment.clearColor.g, app->scene.environment.clearColor.b, 1.0f);


    if (drawWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    //regular mesh rendering logic
    for (auto entity : view) {
        TransformComponent transformComponent = app->scene.registry.get<TransformComponent>(entity);
        UpdateTransform(transformComponent.transform);
        Mesh mesh = app->scene.registry.get<MeshComponent>(entity).mesh;

        if (!mesh.cullBackface) {
            glDisable(GL_CULL_FACE);
        }

        glBindVertexArray(mesh.vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glUseProgram(mesh.material.shader->programId);

        UploadShaderUniformInt(mesh.material.shader->programId, "receivesShadow", mesh.receivesShadow);
        UploadShaderUniformInt(mesh.material.shader->programId, "shadowMap", 1);

        UploadTransformationDataUniforms(mesh, transformComponent.transform.matrix, camera.view, camera.projection, lightView, lightProjection);
        UploadMaterialUniforms(mesh);

        //upload environment data
        UploadShaderUniformVec3(mesh.material.shader->programId, "sunDirection", app->scene.environment.sunDirection);
        UploadShaderUniformVec3(mesh.material.shader->programId, "sunColor", app->scene.environment.ambientColor);
        UploadShaderUniformVec3(mesh.material.shader->programId, "cameraPosition", camera.position);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowFramebuffer.depthTexture.id);

        app->scene.InsertDrawLogic(mesh, entity);

        if (!mesh.material.shader->tesselationControlShaderPath.empty() && !mesh.material.shader->tesselationEvaluationShaderPath.empty()) {
            if (!mesh.indices.empty()) {
                glDrawElements(GL_PATCHES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
            }
            else {
                glDrawArrays(GL_PATCHES, 0, mesh.vertices.size() / 3);
            }
        }
        else {
            if (!mesh.indices.empty()) {
                glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
            }
            else {
                glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size() / 3);
            }
        }

        glUseProgram(0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glBindVertexArray(0);

        if (!mesh.cullBackface) {
            glEnable(GL_CULL_FACE);
        }
    }

    //instanced rendering loop
    for (auto entity : view2) {
        InstancedMeshComponent& instancedMeshComponent = view2.get<InstancedMeshComponent>(entity);
        Mesh mesh = instancedMeshComponent.mesh;

        if ((drawGrass && instancedMeshComponent.mesh.material.shader->programId == grassInstancedShader.programId) || instancedMeshComponent.mesh.material.shader->programId != grassInstancedShader.programId) {
            if (!mesh.cullBackface) {
                glDisable(GL_CULL_FACE);
            }

            if (mesh.vao == 0) {
                Application::Get()->logger.ThrowRuntimeError("MAJOR ERROR: attempting to draw a mesh that has no VAO (you probably forgot to call Renderer::CreateMeshBuffers() somwhere");
            }

            glBindVertexArray(mesh.vao);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glUseProgram(mesh.material.shader->programId);

            UploadShaderUniformInt(mesh.material.shader->programId, "receivesShadow", mesh.receivesShadow);
            UploadShaderUniformInt(mesh.material.shader->programId, "shadowMap", 1);

            UploadTransformationDataUniforms(mesh, glm::identity<glm::mat4>(), camera.view, camera.projection, lightView, lightProjection);
            UploadMaterialUniforms(mesh);

            //upload environment data
            UploadShaderUniformFloat(mesh.material.shader->programId, "time", (float)app->clock.time);
            UploadShaderUniformVec3(mesh.material.shader->programId, "sunDirection", app->scene.environment.sunDirection);
            UploadShaderUniformVec3(mesh.material.shader->programId, "sunColor", app->scene.environment.ambientColor);
            UploadShaderUniformVec3(mesh.material.shader->programId, "cameraPosition", camera.position);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, shadowFramebuffer.depthTexture.id);

            app->scene.InsertInstancedDrawLogic(mesh, entity);

            if (!mesh.indices.empty()) {
                glDrawElementsInstanced(GL_TRIANGLES,  mesh.indices.size(), GL_UNSIGNED_INT, 0, instancedMeshComponent.transforms.size());
            }
            else {
                glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.vertices.size() / 3, instancedMeshComponent.transforms.size());
            }

            glUseProgram(0);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            glDisableVertexAttribArray(3);
            glBindVertexArray(0);

            if (!mesh.cullBackface) {
                glEnable(GL_CULL_FACE);
            }
        }
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    if (showShadowMapDebug) {
        glBindVertexArray(fullscreenQuad.vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(2);
        glUseProgram(fullscreenQuad.material.shader->programId);

        if (fullscreenQuad.material.texture.id != 0) {
            UploadShaderUniformInt(fullscreenQuad.material.shader->programId, "hasBaseTexture", 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, shadowFramebuffer.depthTexture.id);
        }
        else {
            UploadShaderUniformInt(fullscreenQuad.material.shader->programId, "hasBaseTexture", 0);
        }

        glDrawElements(GL_TRIANGLES, fullscreenQuad.indices.size(), GL_UNSIGNED_INT, 0);

        glUseProgram(0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(2);
        glBindVertexArray(0);
    }

}

void Renderer::CleanUp() {
    DeleteShader(opaqueLitShader);
    DeleteShader(opaqueLitInstancedShader);
    DeleteShader(postpassShader);
    DeleteShader(fullscreenQuadShader);
    DeleteShader(terrainShader);
}
