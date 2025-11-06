#version 430 core

#include <transformation_data.glsl>

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in mat4 aTransform;

uniform TransformationData transformationData;

uniform int applyWind;
uniform float time;

layout(location = 0) out vec3 pPosition;
layout(location = 1) out vec3 pNormal;
layout(location = 2) out vec2 pUV;
layout(location = 3) out vec4 fragPosLightSpace;

#include <shader_utils.glsl>

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(aTransform)));

    vec4 worldPosition = aTransform * vec4(aPosition, 1.0f);
    if (applyWind != 0) {
        vec3 windPosition = aPosition + sin(time + length(aPosition.xz)) / 25.0f;
        worldPosition = aTransform * vec4(windPosition, 1.0f);
    }

    gl_Position = transformationData.projection * transformationData.view * worldPosition;

    pNormal = normalize(normalMatrix * aNormal).xyz;
    pUV = aUV;
    pPosition = (aTransform * vec4(aPosition, 1.0f)).xyz;
    fragPosLightSpace = transformationData.lightProjection * transformationData.lightView * worldPosition;
}
