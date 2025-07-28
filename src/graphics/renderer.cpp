#include "renderer.hpp"
#include "tiny_obj_loader.h"
#include <cassert>
#include <iostream>
#include <regex>
#include <glad/glad.h>
#include "../globals.hpp"
#include "glm.hpp"
#include "ext/matrix_clip_space.hpp"
#include "ext/matrix_transform.hpp"
#include "gtc/type_ptr.inl"

void Renderer::CreateMeshBuffers(Mesh& mesh) {
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

Multimesh Renderer::LoadMeshAsset(std::string meshAssetPath) {
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

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::vector<Mesh> meshes = {};
    meshes.resize(shapes.size());

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                meshes[s].vertices.push_back(vx);
                meshes[s].vertices.push_back(vy);
                meshes[s].vertices.push_back(vz);

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                    meshes[s].normals.push_back(nx);
                    meshes[s].normals.push_back(ny);
                    meshes[s].normals.push_back(nz);
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                    meshes[s].uvs.push_back(tx);
                    meshes[s].uvs.push_back(ty);
                }

                // Optional: vertex colors
                // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }

    for (Mesh& mesh : meshes) {
        CreateMeshBuffers(mesh);
        mesh.material.shaderProgramId = globals.renderer.meshLitShader.programId;
    }

    globals.logger.Log("successfully loaded obj file: " + meshAssetPath);
    return {.meshes = meshes};
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

void Renderer::LoadShaders() {
    meshLitShader = Renderer::CreateShader("resources/shaders/mesh_lit.vert", "resources/shaders/mesh_lit.frag");
}

void Renderer::UpdateCameraMatrices() {
    camera.projection = glm::identity<glm::mat4>();
    camera.projection = glm::perspective<float>(camera.fov, (float)globals.window.width / (float)globals.window.height, camera.near, camera.far);

    camera.view = glm::identity<glm::mat4>();
    camera.view = glm::lookAt(camera.position, camera.target, camera.up);
}

void Renderer::UpdateMultimeshMatrices(Multimesh &multimesh) {
    multimesh.transform = glm::identity<glm::mat4>();
    multimesh.transform = glm::scale(multimesh.transform, multimesh.scale);
    multimesh.transform = glm::rotate(multimesh.transform, multimesh.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    multimesh.transform = glm::rotate(multimesh.transform, multimesh.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    multimesh.transform = glm::rotate(multimesh.transform, multimesh.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    multimesh.transform = glm::translate(multimesh.transform, multimesh.position);
}

void Renderer::UploadMesh3DMatrices(Mesh& mesh, glm::mat4& transform) {
    UploadShaderUniformMat4(mesh.material.shaderProgramId, "transform", transform);
    UploadShaderUniformMat4(mesh.material.shaderProgramId, "view", camera.view);
    UploadShaderUniformMat4(mesh.material.shaderProgramId, "projection", camera.projection);
}

void Renderer::UploadMaterialUniforms(Mesh &mesh) {
    UploadShaderUniformVec3(mesh.material.shaderProgramId, "albedo", mesh.material.albedo);
}

void Renderer::Initialize() {
    LoadShaders();

    UpdateCameraMatrices();
}

void Renderer::DrawActiveScene() {
    UpdateCameraMatrices();

    for (Multimesh& multimesh : globals.scene.meshes) {
        UpdateMultimeshMatrices(multimesh);

        glEnable(GL_DEPTH_TEST);
        for (Mesh& mesh : multimesh.meshes) {
            glBindVertexArray(mesh.vao);
            glEnableVertexAttribArray(0);
            glUseProgram(mesh.material.shaderProgramId);

            UploadMesh3DMatrices(mesh, multimesh.transform);
            UploadMaterialUniforms(mesh);

            glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());

            glUseProgram(0);
            glDisableVertexAttribArray(0);
            glBindVertexArray(0);
        }
    }
}

void Renderer::CleanUp() {
    DeleteShader(meshLitShader);
}
