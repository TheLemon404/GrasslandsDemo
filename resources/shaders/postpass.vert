#version 330 core

const vec2 positions[4] = vec2[](
    vec2(-1.0,  1.0),
    vec2(-1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2( 1.0, -1.0)
);

const vec2 uvs[4] = vec2[](
    vec2(0.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0)
);

layout (location = 0) out vec2 pUV;

void main() {
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
    pUV = uvs[gl_VertexID];
}