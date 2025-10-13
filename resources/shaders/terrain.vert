#version 410 core

#include <transformation_data.glsl>

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform TransformationData transformationData;

uniform sampler2D heightMap;
uniform float heightMapStrength;

layout (location = 0) out vec3 pPosition;
layout (location = 1) out vec3 pNormal;
layout (location = 2) out vec2 pUV;
layout (location = 3) out vec4 fragPosLightSpace;

void main()
{
    vec4 worldPosition = transformationData.transform * vec4(aPosition, 1.0f);
    mat3 normalMatrix = mat3(transpose(inverse(transformationData.transform)));
    vec4 modifiedPosition = (worldPosition + vec4(0.0f, texture(heightMap, aUV).r * heightMapStrength, 0.0f, 0.0f));

    gl_Position = transformationData.projection * transformationData.view * transformationData.transform * modifiedPosition;
    pUV = aUV;
    pPosition = (transformationData.transform * modifiedPosition).xyz;
    fragPosLightSpace = transformationData.lightProjection * transformationData.lightView * transformationData.transform * modifiedPosition;
}