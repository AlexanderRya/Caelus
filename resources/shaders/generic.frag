#version 460

layout (location = 0) out vec4 frag_color;

layout (location = 1) uniform sampler2D textures[];

void main() {
    frag_color = vec4(1.0);
}
