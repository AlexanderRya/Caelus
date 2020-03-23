#version 460

layout (location = 0) in vec3 ivertex_pos;

layout (location = 0) out vec2 ouv_coords;

layout (set = 0, binding = 0) uniform Camera {
    mat4 pv_mat;
} camera;

layout (std430, set = 0, binding = 1) buffer readonly Instance {
    mat4 model[];
} instances;

void main() {
    gl_Position = camera.pv_mat * instances.model[gl_InstanceIndex] * vec4(ivertex_pos, 1.0f);
}