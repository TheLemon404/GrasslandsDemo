#pragma once

struct Texture {
    unsigned int id;
    int width, height;
    int nrChannels = 4;

    static Texture LoadTextureFromFile(const char* path, int textureFormat = 4);
};
