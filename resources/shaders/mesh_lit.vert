#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) out vec3 pPosition;
layout (location = 1) out vec3 pNormal;
layout (location = 2) out vec2 pUV;

void main()
{
    gl_Position = projection * view * transform * vec4(aPosition, 1.0);
    pNormal = aNormal;
    pUV = aUV;
}