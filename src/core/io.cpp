#include "io.hpp"
#include "../application.hpp"
#include <fstream>
#include <iosfwd>
#include <iostream>

const char* IO::LoadShaderFileContents(const char* localPath) {
    std::ifstream::pos_type size;
    char * memblock;
    std::string text;

    // file read based on example in cplusplus.com tutorial
    std::ifstream file (localPath, std::ios::in| std::ios::binary| std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char [1 + size];
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        file.close();
        memblock[size] = '\0';
        Application::Get()->logger.Log("file " + std::string(localPath) + " loaded");
        text.assign(memblock);
    }
    else
    {
        Application::Get()->logger.Log("Unable to open file " + std::string(localPath));
        exit(1);
    }
    return memblock;
}
