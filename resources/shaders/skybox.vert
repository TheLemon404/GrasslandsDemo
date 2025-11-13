#version 430 core
layout(location = 0) in vec3 aPosition;

layout(location = 0) out vec3 pUV;

uniform mat4 projection;
uniform mat4 view;

void main() {
    pUV = aPosition;
    gl_Position = projection * view * vec4(aPosition, 1.0f);
}
