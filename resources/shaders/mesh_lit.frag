#version 330 core

uniform vec3 albedo;

out vec4 FragColor;

void main()
{
    FragColor = vec4(albedo, 1.0f);
}