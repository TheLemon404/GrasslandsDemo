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
layout (location = 1) out vec3 pNormal;
layout (location = 2) out vec2 pUV;
layout (location = 3) out vec4 fragPosLightSpace;

void main()
{
    vec4 worldPosition = transform * vec4(aPosition, 1.0f);
    mat3 normalMatrix = mat3(transpose(inverse(transform)));
    vec4 modifiedPosition = (worldPosition + vec4(0.0f, texture(heightMap, aUV).r * heightMapStrength, 0.0f, 0.0f));

    gl_Position = projection * view * transform * modifiedPosition;
    pUV = aUV;
    pPosition = (transform * modifiedPosition).xyz;
    fragPosLightSpace = lightProjection * lightView * transform * modifiedPosition;
}