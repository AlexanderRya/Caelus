#version 460

layout (location = 0) in vec3 ivertex_pos;
layout (location = 1) in vec2 itexture_coords;

layout (location = 0) out vec2 otexture_coords;
layout (location = 1) out vec3 ocolor;

layout (set = 0, binding = 0) uniform CameraData {
    mat4 pv_matrix;
} camera;

layout (set = 0, binding = 1) buffer readonly Instance {
    mat4 models[];
} instances;

layout (set = 0, binding = 2) buffer readonly ColorData {
    vec3 colors[];
} color_instances;

void main() {
    gl_Position = camera.pv_matrix * instances.models[gl_InstanceIndex] * vec4(ivertex_pos, 1.0f);
    otexture_coords = itexture_coords;
    ocolor = color_instances.colors[gl_InstanceIndex];
}