#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout (location = 0) in vec2 uvs;
layout (location = 1) in vec3 normals;

layout (location = 0) out vec4 frag_color;

layout (set = 0, binding = 2) uniform Material {
    vec4 color;
    int texture_idx;
};

layout (set = 0, binding = 3) uniform sampler2D textures[];

void main() {
    if (texture_idx == -1) {
        frag_color = vec4(color);
    } else {
        frag_color = vec4(color * texture(textures[texture_idx], uvs));
    }
}
