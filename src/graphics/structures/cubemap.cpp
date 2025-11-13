#include "cubemap.hpp"
#include "../../application.hpp"
#include "../utils/stb_image.h"
#include <glad/glad.h>

Cubemap Cubemap::LoadCubemapTextureFromFile(const char *path) {
    Cubemap result = {};

    glGenTextures(1, &result.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, result.id);

    void* offset;
    unsigned char* data = stbi_load(path, &result.totalWidth, &result.totalHeight, &result.numChannels, 0);
    Application::Get()->logger.Log(std::to_string(result.totalWidth));
    Application::Get()->logger.Log(std::to_string(result.totalHeight));

    result.width = result.totalWidth / 4;
    result.height = result.totalHeight / 3;

    if(data){
        //+X
        //glPixelStorei(GL_UNPACK_SKIP_PIXELS, 2 * result.width);
        //glPixelStorei(GL_UNPACK_SKIP_ROWS, 1 * result.height);
        //offset = data + (2 * result.width * result.numChannels) + (1 * result.height * result.totalWidth * result.numChannels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, result.totalWidth, result.totalHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        //-X
        //glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0 * result.width);
        //glPixelStorei(GL_UNPACK_SKIP_ROWS, 1 * result.height);
        //offset = data + (0 * result.width * result.numChannels) + (1 * result.height * result.totalWidth * result.numChannels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, result.totalWidth, result.totalHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        //+Y
        //glPixelStorei(GL_UNPACK_SKIP_PIXELS, 1 * result.width);
        //glPixelStorei(GL_UNPACK_SKIP_ROWS, 0 * result.height);
        //offset = data + (1 * result.width * result.numChannels) + (0 * result.height * result.totalWidth * result.numChannels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, result.totalWidth, result.totalHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        //-Y
        //glPixelStorei(GL_UNPACK_SKIP_PIXELS, 1 * result.width);
        //glPixelStorei(GL_UNPACK_SKIP_ROWS, 2 * result.height);
        //offset = data + (1 * result.width * result.numChannels) + (2 * result.height * result.totalWidth * result.numChannels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, result.totalWidth, result.totalHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        //+Z
        //glPixelStorei(GL_UNPACK_SKIP_PIXELS, 1 * result.width);
        //glPixelStorei(GL_UNPACK_SKIP_ROWS, 1 * result.height);
        //offset = data + (1 * result.width * result.numChannels) + (1 * result.height * result.totalWidth * result.numChannels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, result.totalWidth, result.totalHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        //-Z
        //glPixelStorei(GL_UNPACK_SKIP_PIXELS, 3 * result.width);
        //glPixelStorei(GL_UNPACK_SKIP_ROWS, 1 * result.height);
        //offset = data + (3 * result.width * result.numChannels) + (1 * result.height * result.totalWidth * result.numChannels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, result.totalWidth, result.totalHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        Application::Get()->logger.Log("Successfully loaded cubemap texture: " + std::string(path));
    }
    else{
        Application::Get()->logger.Log("Failed to load cubemap texture: " + std::string(path));
    }

    stbi_image_free(data);

    return result;
}
