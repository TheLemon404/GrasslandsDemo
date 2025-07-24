cmake_minimum_required(VERSION 3.31)
project(Chess3D)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(submodules/glfw)
add_subdirectory(submodules/quil)

file(GLOB_RECURSE src CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
add_executable(Chess3D ${src})

target_link_libraries(Chess3D PRIVATE QuIL glfw)
