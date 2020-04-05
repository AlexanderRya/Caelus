#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout (location = 0) in vec2 uvs;
layout (location = 1) in vec3 normals;

layout (location = 0) out vec4 frag_color;

struct Material {
    vec4 color;
    int texture_idx;
};

layout (std430, set = 0, binding = 2) buffer readonly Materials {
    Material material[];
};

layout (set = 0, binding = 3) uniform sampler2D textures[];

layout (push_constant) uniform Indices {
    int transform_idx;
    int material_idx;
};

void main() {
    Material current_material = material[material_idx];

    if (current_material.texture_idx == -1) {
        frag_color = vec4(current_material.color);
    } else {
        frag_color = vec4(current_material.color * texture(textures[current_material.texture_idx], uvs));
    }
}
