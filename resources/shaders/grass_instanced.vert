#version 430 core

#include <transformation_data.glsl>

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(binding = 0, std430) readonly buffer grassSSBO {
    mat4 grassTransformMatrices[];
};

uniform vec2 terrainSpaceUVBounds;

uniform TransformationData transformationData;

uniform float time;
uniform sampler2D perlinTexture;
uniform float breezeAmount;
uniform float windAmount;
uniform float windAngle;

layout(location = 0) out vec3 pPosition;
layout(location = 1) out vec3 pNormal;
layout(location = 2) out vec2 pUV;
layout(location = 3) out vec4 fragPosLightSpace;
layout(location = 4) out vec2 terrainSpaceUV;

#include <matrix_math.glsl>

#include <shader_utils.glsl>

void main()
{
    //grass curving and wind
    vec4 preCurvedWorldPosition = (grassTransformMatrices[gl_InstanceID] * vec4(aPosition, 1.0f));
    terrainSpaceUV = ((vec2(preCurvedWorldPosition.x, preCurvedWorldPosition.z) + terrainSpaceUVBounds) / (terrainSpaceUVBounds * 2.0));

    float wind = texture(perlinTexture, terrainSpaceUV + vec2(time / 15)).r;
    //the breeze factor ensures a psuedo random sway per grass blade
    float breeze = cos(time + preCurvedWorldPosition.x + preCurvedWorldPosition.z + 25 * random(preCurvedWorldPosition.xz));
    float breezeCurveAmount = aUV.y * breezeAmount + (breeze / 15);
    mat4 breezeCurveMatrix = rotateX(breezeCurveAmount + (wind / 3));

    float windCurveAmount = aUV.y * wind;
    mat4 windCurveMatrix = rotateY(windCurveAmount * windAmount);

    vec4 CurvedWorldPosition = (grassTransformMatrices[gl_InstanceID] * (vec4(aPosition, 1.0f) * breezeCurveMatrix * windCurveMatrix));
    mat3 normalMatrix = mat3(transpose(inverse(grassTransformMatrices[gl_InstanceID])));

    gl_Position = transformationData.projection * transformationData.view * CurvedWorldPosition;

    pNormal = normalize(normalMatrix * aNormal).xyz;
    pUV = aUV;
    pPosition = CurvedWorldPosition.xyz;
    fragPosLightSpace = transformationData.lightProjection * transformationData.lightView * CurvedWorldPosition;
}
