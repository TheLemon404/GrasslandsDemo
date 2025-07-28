#version 330 core

layout (location = 0) in vec3 pPosition;
layout (location = 1) in vec3 pNormal;
layout (location = 2) in vec2 pUV;

uniform vec3 albedo;

out vec4 FragColor;

void main()
{
    FragColor = vec4(pNormal, 1.0f);
}