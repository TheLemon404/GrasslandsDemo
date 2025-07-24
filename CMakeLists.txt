cmake_minimum_required(VERSION 3.31)
project(Chess3D)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(submodules/glfw)
add_subdirectory(submodules/quil)

file(GLOB_RECURSE src CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
add_executable(Chess3D ${src} external/glad/src/glad.c
        src/graphics/renderer.hpp
        src/core/io.hpp
        src/core/io.cpp
        src/graphics/renderer.cpp)
target_include_directories(Chess3D PRIVATE external/glad/include)

target_link_libraries(Chess3D PRIVATE QuIL glfw)
