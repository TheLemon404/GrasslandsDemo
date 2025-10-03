#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 2) in vec2 aUV;

layout (location = 0) out vec2 pUV;

void main()
{
    gl_Position = vec4(aPosition, 1.0f);
    pUV = aUV;
}