#pragma once

#include "texture.hpp"
struct Cubemap {
  unsigned int id;
  int totalWidth, totalHeight, width, height, numChannels = 4;

  static Cubemap LoadCubemapTextureFromFile(const char *path);
};
