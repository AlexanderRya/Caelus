#version 460
#extension GL_EXT_nonuniform_qualifier : enable

struct MaterialInstance {
    vec4 color;
    int texture_idx;
};

layout (location = 0) in vec2 uvs;
layout (location = 1) in flat uint instance_index;

layout (location = 0) out vec4 frag_color;

layout (std430, set = 0, binding = 2) buffer readonly Material {
    MaterialInstance materials[];
};

layout (set = 0, binding = 3) uniform sampler2D textures[];

void main() {
    if (materials[instance_index].texture_idx == -1) {
        frag_color = vec4(materials[instance_index].color);
    } else {
        frag_color = vec4(materials[instance_index].color * texture(textures[materials[instance_index].texture_idx], uvs));
    }
}
