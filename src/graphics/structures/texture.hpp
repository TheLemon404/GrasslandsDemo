#pragma once

struct Texture {
    unsigned int id;
    int width, height;
    int numChannels = 4;

    static Texture LoadTextureFromFile(const char* path, int textureFormat = 4, bool repeat = true, bool pointFilter = false);
};
