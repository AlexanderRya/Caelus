#version 460

layout (location = 0) in vec3 ivertex_pos;
layout (location = 1) in vec3 inormals;
layout (location = 2) in vec2 iuvs;

layout (location = 0) out vec2 uvs;

layout (set = 0, binding = 0) uniform Camera {
    mat4 pv_mat;
} camera;

layout (std430, set = 0, binding = 1) buffer readonly InstanceBuf {
    mat4 instances[];
};

void main() {
    gl_Position = camera.pv_mat * instances[gl_InstanceIndex] * vec4(ivertex_pos, 1.0);
    uvs = iuvs;
}