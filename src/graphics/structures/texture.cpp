#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../utils/stb_image.h"
#include "glad/glad.h"
#include "../../application.hpp"

Texture Texture::LoadTextureFromFile(const char *path, int textureFormat, bool repeat) {
    Texture texture = {
        .nrChannels = textureFormat,
    };

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    if (repeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *data = stbi_load(path, &texture.width, &texture.height, &texture.nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (texture.nrChannels == 1)
            format = GL_RED;
        else if (texture.nrChannels == 3)
            format = GL_RGB;
        else if (texture.nrChannels == 4)
            format = GL_RGBA;
        else {
            application.logger.Log("Unsupported number of channels: " + std::to_string(texture.nrChannels));
            stbi_image_free(data);
            return texture;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        application.logger.Log("Successfully loaded texture: " + std::string(path));
    }
    else {
        application.logger.Log("Failed to load texture: " + std::string(path));
    }
    stbi_image_free(data);

    return texture;
}
