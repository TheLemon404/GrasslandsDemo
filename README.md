# Foliage Rendering Demo
  An OpenGL graphics demo capable of rendering large amounts of foliage using instanced rendering, compute shaders, and tessellation shaders

# Details
* Utilizes SSBO to store instance transforms
* Instance transform SSBO is dynamically populated by a compute shader
* Grass wind effects are applied in the vertex shader through noise textures
* Terrain is created from a heightmap, and uses a tessellation shader to allow for distance-based level-of-detail

> [!IMPORTANT]  
> This project is actively in development. This is not the finished project, and I am currently building this demo in my free time.

# Screenshots
<img width="1188" height="821" alt="Screenshot 2025-11-06 204726" src="https://github.com/user-attachments/assets/26792a1f-be75-4e7f-91a8-f46d897b9f00" />
<img width="3439" height="1368" alt="image" src="https://github.com/user-attachments/assets/b16311f2-e5b8-4358-a844-728dd1594e2b" />
<img width="1187" height="817" alt="Screenshot 2025-11-06 204801" src="https://github.com/user-attachments/assets/cf69984f-82bc-403e-8ed8-7cbe12c14117" />
<img width="1919" height="1001" alt="Screenshot 2025-11-02 084233" src="https://github.com/user-attachments/assets/e665205a-a069-4dab-9c4a-fd19aff336dd" />

# Lessons Learned
* Compute Shaders are an EXTREMELY useful tool when in need of massively parallel computing
* Vulkan's default use of indirect rendering would make a project like this substantially more performant
* Rendering foliage efficiently is not just about culling, but also relies on only storing necessary data in memory (like not storing grass locations that are beyond the camera's FOV, but being able to load them in quickly when they are in the FOV)

# Building from source
* download project source and run `git submodule update`
* install msys32: https://www.msys2.org/
* using the msys32 terminal, install cmake, ninja, and mingw with the commands:
* - `pacman -S mingw-w64-x86_64-cmake`
* - `pacman -S mingw-w64-x86_64-ninja`
* - `pacman -S mingw-w64-ucrt-x86_64-gcc`
* confugure CMake project with: `C:/msys64/mingw64/bin/cmake.exe -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=C:/msys64/mingw64/bin/ninja.exe -G Ninja -B cmake-build-debug`
* build project with: `C:/msys64/mingw64/bin/cmake.exe --build cmake-build-debug --target GrasslandsDemo`
* run project with: `./cmake-build-debug/GrasslandsDemo.exe`
