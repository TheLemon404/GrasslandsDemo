#include "renderer.hpp"

#include <algorithm>

#include "tiny_obj_loader.h"
#include <cassert>
#include <regex>
#include <glad/glad.h>
#include "../globals.hpp"
#include "glm.hpp"
#include "ext/matrix_clip_space.hpp"
#include "ext/matrix_transform.hpp"
#include "gtc/type_ptr.inl"

void Renderer::CreateMeshBuffers(Mesh& mesh) {
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &mesh.nbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.nbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &mesh.uvbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.uvbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.uvs.size() * sizeof(float), mesh.uvs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
}

Mesh Renderer::LoadMeshSubAsset(std::string meshAssetPath, int subMeshIndex, tinyobj::ObjReader& reader) {
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

            result.vertices.push_back(vx);
            result.vertices.push_back(vy);
            result.vertices.push_back(vz);

            // Check if `normal_index` is zero or positive. negative = no normal data
            if (idx.normal_index >= 0) {
                tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                result.normals.push_back(nx);
                result.normals.push_back(ny);
                result.normals.push_back(nz);
            }

            // Check if `texcoord_index` is zero or positive. negative = no texcoord data
            if (idx.texcoord_index >= 0) {
                tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                result.uvs.push_back(tx);
                result.uvs.push_back(ty);
            }

            // Optional: vertex colors
            // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
            // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
            // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
        }
        index_offset += fv;

        for (int i = 0; i < shapes[subMeshIndex].mesh.indices.size(); i++) {
            tinyobj::index_t idx = shapes[subMeshIndex].mesh.indices[i];
            result.indices.push_back(i);
        }

        // per-face material
        shapes[subMeshIndex].mesh.material_ids[f];
    }

    return result;
}

Multimesh Renderer::LoadMultimeshAsset(std::string meshAssetPath) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = std::regex_replace(meshAssetPath, std::regex(".obj\n"), ".mtl");

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(meshAssetPath.c_str(), reader_config)) {
        if (!reader.Error().empty()) {
            globals.logger.ThrowRuntimeError("TinyObjReader: " + reader.Error());
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        globals.logger.Log("TinyObjReader: " + reader.Warning());
    }

    auto& shapes = reader.GetShapes();

    std::vector<Mesh> meshes = {};
    meshes.resize(shapes.size());

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        meshes[s] = LoadMeshSubAsset(meshAssetPath, s, reader);
    }

    for (Mesh& mesh : meshes) {
        CreateMeshBuffers(mesh);
        mesh.material.shaderProgramId = globals.renderer.opaqueLitShader.programId;
    }

    globals.logger.Log("successfully loaded obj file: " + meshAssetPath);
    return {.meshes = meshes};
}

Instancedmesh Renderer::LoadInstancedmeshAsset(std::string meshAssetPath, std::vector<Transform> transforms) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = std::regex_replace(meshAssetPath, std::regex(".obj\n"), ".mtl");

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(meshAssetPath.c_str(), reader_config)) {
        if (!reader.Error().empty()) {
            globals.logger.ThrowRuntimeError("TinyObjReader: " + reader.Error());
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        globals.logger.Log("TinyObjReader: " + reader.Warning());
    }

    Mesh mesh = LoadMeshSubAsset(meshAssetPath, 0, reader);

    for (int i = 0; i < transforms.size(); i++) {
        UpdateTransform(transforms[i]);
    }

    CreateMeshBuffers(mesh);
    mesh.material.shaderProgramId = globals.renderer.opaqueInstancedLitShader.programId;

    Instancedmesh result = {
        .mesh = mesh,
        .transforms = transforms
    };

    return result;
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

Shader Renderer::CreateShader(std::string vertexShaderLocalPath, std::string fragmentShaderLocalPath) {
    const char* vertexShaderContent = globals.io.LoadShaderFileContents(vertexShaderLocalPath.c_str());
    const char* fragmentShaderContent = globals.io.LoadShaderFileContents(fragmentShaderLocalPath.c_str());

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
        globals.logger.ThrowRuntimeError(vertexInfoLog);
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
        globals.logger.ThrowRuntimeError(fragmentInfoLog);
    }

    programId = glCreateProgram();

    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    int programSuccess;
    char programInfoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &programSuccess);
    if(!programSuccess) {
        glGetProgramInfoLog(programId, 512, nullptr, programInfoLog);
        globals.logger.ThrowRuntimeError(programInfoLog);
    }

    globals.logger.Log("successfully compiled vertex shader: " + std::string(vertexShaderLocalPath));
    globals.logger.Log("successfully compiled fragment shader: " + std::string(fragmentShaderLocalPath));

    return {
        vertexShaderContent,
        fragmentShaderContent,
        vertexShaderId,
        fragmentShaderId,
        programId
    };
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

Framebuffer Renderer::CreateFramebuffer(unsigned int width, unsigned int height) {
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
        .specularTexture = {
            .width = width,
            .height = height
        },
        .depthStencilRenderbuffer = {
            .width = width,
            .height = height}
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

    glGenTextures(1, &framebuffer.specularTexture.id);
    glBindTexture(GL_TEXTURE_2D, framebuffer.specularTexture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, framebuffer.specularTexture.id, 0);

    glGenRenderbuffers(1, &framebuffer.depthStencilRenderbuffer.id);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depthStencilRenderbuffer.id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depthStencilRenderbuffer.id);

    const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        globals.logger.ThrowRuntimeError("failed to create framebuffer");
    }

    return framebuffer;
}

void Renderer::LoadShaders() {
    opaqueLitShader = CreateShader("resources/shaders/opaque_lit.vert", "resources/shaders/opaque_lit.frag");
    opaqueInstancedLitShader = CreateShader("resources/shaders/opaque_instanced_lit.vert", "resources/shaders/opaque_instanced_lit.frag");
    prepassShader = CreateShader("resources/shaders/postpass.vert", "resources/shaders/postpass.frag");
}

void Renderer::UpdateCameraMatrices() {
    camera.projection = glm::identity<glm::mat4>();
    camera.projection = glm::perspective<float>(camera.fov, (float)globals.window.width / (float)globals.window.height, camera.near, camera.far);

    camera.view = glm::identity<glm::mat4>();
    camera.view = glm::lookAt(camera.position, camera.target, camera.up);
}

void Renderer::UpdateTransform(Transform& transform) {
    transform.matrix = glm::identity<glm::mat4>();
    transform.matrix = glm::scale(transform.matrix, transform.scale);
    transform.matrix = glm::rotate(transform.matrix, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    transform.matrix = glm::rotate(transform.matrix, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    transform.matrix = glm::rotate(transform.matrix, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    transform.matrix = glm::translate(transform.matrix, transform.position);
}

void Renderer::UploadMesh3DMatrices(Mesh& mesh, glm::mat4& transform) {
    UploadShaderUniformMat4(mesh.material.shaderProgramId, "transform", transform);
    UploadShaderUniformMat4(mesh.material.shaderProgramId, "view", camera.view);
    UploadShaderUniformMat4(mesh.material.shaderProgramId, "projection", camera.projection);
}

void Renderer::UploadMaterialUniforms(Mesh &mesh) {
    UploadShaderUniformVec3(mesh.material.shaderProgramId, "albedo", mesh.material.albedo);
    UploadShaderUniformFloat(mesh.material.shaderProgramId, "specular", mesh.material.specular);
    if (mesh.material.texture.width == 0 && mesh.material.texture.height == 0) {
        UploadShaderUniformInt(mesh.material.shaderProgramId, "haseBaseTexture", 0);
    }
    else {
        UploadShaderUniformInt(mesh.material.shaderProgramId, "haseBaseTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.material.texture.id);
    }
}

void Renderer::Initialize() {
    LoadShaders();

    framebuffer = CreateFramebuffer(globals.window.width, globals.window.height);

    UpdateCameraMatrices();
}

void Renderer::DrawActiveScene() {
    UpdateCameraMatrices();

    //first pass
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
    glViewport(0, 0, framebuffer.colorTexture.width, framebuffer.colorTexture.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    for (Multimesh& multimesh : globals.scene.meshes) {
        UpdateTransform(multimesh.transform);

        for (Mesh& mesh : multimesh.meshes) {
            glBindVertexArray(mesh.vao);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glUseProgram(mesh.material.shaderProgramId);

            UploadMesh3DMatrices(mesh, multimesh.transform.matrix);
            UploadMaterialUniforms(mesh);

            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);

            glUseProgram(0);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            glBindVertexArray(0);
        }
    }

    for (Instancedmesh instancedmesh : globals.scene.instancedmeshes) {
        glBindVertexArray(instancedmesh.mesh.vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glUseProgram(instancedmesh.mesh.material.shaderProgramId);

        for (int i = 0; i < instancedmesh.transforms.size(); i++) {
            UpdateTransform(instancedmesh.transforms[i]);
            UploadShaderUniformMat4(instancedmesh.mesh.material.shaderProgramId, "transforms[" + std::to_string(i) + "]", instancedmesh.transforms[i].matrix);
        }

        UploadShaderUniformMat4(instancedmesh.mesh.material.shaderProgramId, "view", camera.view);
        UploadShaderUniformMat4(instancedmesh.mesh.material.shaderProgramId, "projection", camera.projection);
        UploadMaterialUniforms(instancedmesh.mesh);

        glDrawElementsInstanced(GL_TRIANGLES, instancedmesh.mesh.indices.size(), GL_UNSIGNED_INT, 0, instancedmesh.transforms.size());

        glUseProgram(0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glBindVertexArray(0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //second pass
    glViewport(0, 0, globals.window.width, globals.window.height);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //upload framebuffer textures
    glUseProgram(prepassShader.programId);
    glUniform1i(glGetUniformLocation(prepassShader.programId, "colorTexture"), 0);
    glUniform1i(glGetUniformLocation(prepassShader.programId, "normalTexture"), 1);
    glUniform1i(glGetUniformLocation(prepassShader.programId, "positionTexture"), 2);
    glUniform1i(glGetUniformLocation(prepassShader.programId, "specularTexture"), 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffer.colorTexture.id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, framebuffer.normalTexture.id);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, framebuffer.positionTexture.id);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, framebuffer.specularTexture.id);

    //upload environment data
    UploadShaderUniformVec3(prepassShader.programId, "sunDirection", globals.scene.environment.sunDirection);
    UploadShaderUniformVec3(prepassShader.programId, "sunColor", globals.scene.environment.sunColor);
    UploadShaderUniformVec3(prepassShader.programId, "ambientColor", globals.scene.environment.ambientColor);
    UploadShaderUniformVec3(prepassShader.programId, "cameraPosition", camera.position);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glUseProgram(0);
}

void Renderer::CleanUp() {
    DeleteShader(opaqueLitShader);
}
