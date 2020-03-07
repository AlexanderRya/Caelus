#version 460

layout (location = 0) in vec2 texture_coords;
layout (location = 1) in vec3 color;

layout (location = 0) out vec4 frag_color;

layout (set = 0, binding = 3) uniform sampler2D default_sampler;

void main() {
    frag_color = vec4(color, 1.0) * texture(default_sampler, texture_coords);
}
