#version 460

layout (location = 0) in vec3 ivertex_pos;

layout (set = 0, binding = 1) buffer readonly Instance {
    mat4 model[];
} instances;

void main() {
    gl_Position =  instances.model[gl_InstanceIndex] * vec4(ivertex_pos, 1.0f);
}