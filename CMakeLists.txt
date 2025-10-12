cmake_minimum_required(VERSION 3.31)
project(Chess3D)

set(CMAKE_CXX_STANDARD 20)

#for imgui
file(GLOB imgui_src CONFIGURE_DEPENDS submodules/imgui/*.cpp submodules/imgui/*.h)

add_subdirectory(submodules/glfw)
add_subdirectory(submodules/quil)
add_subdirectory(submodules/tinyobjloader)
add_subdirectory(submodules/glm)
add_subdirectory(submodules/entt)

file(GLOB_RECURSE src CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
add_executable(Chess3D
        ${src}
        ${imgui_src}
        submodules/imgui/backends/imgui_impl_glfw.cpp
        submodules/imgui/backends/imgui_impl_glfw.h
        submodules/imgui/backends/imgui_impl_opengl3.cpp
        submodules/imgui/backends/imgui_impl_opengl3.h
        external/glad/src/glad.c
)

target_include_directories(Chess3D PRIVATE external/glad/include submodules/glm/glm submodules/imgui)

target_link_libraries(Chess3D PRIVATE QuIL glfw tinyobjloader glm EnTT::EnTT)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})