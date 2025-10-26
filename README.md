# Foliage Rendering Demo
  An OpenGL graphics demo capable of rendering large amounts of foliage using instanced rendering, compute shaders, and tessellation shaders

# Details
* Utilizes SSBO to store instance transforms
* Instance transform SSBO is dynamically populated by a compute shader
* Grass wind effects are applied in the vertex shader through noise textures
* Terrain is created from a heightmap, and uses a tessellation shader to allow for distance-based level-of-detail

# Lessons Learned
* Compute Shaders are an EXTREMELY useful tool when in need of massively parallel computing
* Vulkan's default use of indirect rendering would make a project like this substancially more performant
* Rendering foliage is efficiently is not just about culling, but also relies on only storing necessary data in memory (like not storing grass locations that are beyond the cameras FOV, but being able to load them in quickly when they are in the FOV)

# Building from source
* download project source and run `git submodule update`
* install msys32: https://www.msys2.org/
* using the msys32 termainal, install cmake, ninja, and mingw with the commands:
* - `pacman -S mingw-w64-x86_64-cmake`
* - `pacman -S mingw-w64-x86_64-ninja`
* - `pacman -S mingw-w64-ucrt-x86_64-gcc`
* confugure CMake project with: `C:/msys64/mingw64/bin/cmake.exe -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=C:/msys64/mingw64/bin/ninja.exe -G Ninja -B cmake-build-debug`
* build project with: `C:/msys64/mingw64/bin/cmake.exe --build cmake-build-debug --target GrasslandsDemo`
* run project with: `./cmake-build-debug/GrasslandsDemo.exe`
