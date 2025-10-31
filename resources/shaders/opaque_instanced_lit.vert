#version 430 core

#include <transformation_data.glsl>

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in mat4 aTransform;

uniform TransformationData transformationData;

uniform sampler2D heightMap;
uniform float heightMapStrength;
uniform vec2 terrainSpaceUVBounds;

layout(location = 0) out vec3 pPosition;
layout(location = 1) out vec3 pNormal;
layout(location = 2) out vec2 pUV;
layout(location = 3) out vec4 fragPosLightSpace;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(aTransform)));

    vec4 position = aTransform * vec4(aPosition, 1.0f);
    vec2 terrainSpaceUV = ((vec2(position.x, position.z) + terrainSpaceUVBounds) / (terrainSpaceUVBounds * 2.0));
    position += vec4(0.0f, pow(texture(heightMap, terrainSpaceUV).r, 4) * heightMapStrength, 0.0f, 0.0f);

    gl_Position = transformationData.projection * transformationData.view * position;

    pNormal = normalize(normalMatrix * aNormal).xyz;
    pUV = aUV;
    pPosition = position.xyz;
    fragPosLightSpace = transformationData.lightProjection * transformationData.lightView * position;
}
