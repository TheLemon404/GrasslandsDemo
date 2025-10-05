#version 410 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightView;
uniform mat4 lightProjection;

uniform sampler2D heightMap;
uniform float heightMapStrength;

layout (location = 0) out vec3 pPosition;
layout (location = 1) out vec3 fragWorldPos;
layout (location = 2) out vec2 pUV;
layout (location = 3) out vec4 fragPosLightSpace;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(transform)));
    vec4 modifiedVertexPosition = (vec4(aPosition, 1.0f) + vec4(0.0f, texture(heightMap, aUV).r * heightMapStrength, 0.0f, 0.0f));
    gl_Position = projection * view * transform * modifiedVertexPosition;
    fragWorldPos = (transform * modifiedVertexPosition).xyz;
    pUV = aUV;
    pPosition = (transform * vec4(aPosition, 1.0f)).xyz;
    fragPosLightSpace = lightProjection * lightView * transform * vec4(aPosition, 1.0f);
}