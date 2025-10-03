#version 410 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 transforms[125];
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * transforms[gl_InstanceID] * vec4(aPosition, 1.0f);
}