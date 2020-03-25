#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout (location = 0) in vec2 uvs;

layout (location = 0) out vec4 frag_color;

layout (set = 0, binding = 2) uniform sampler2D textures[];

layout (push_constant) uniform Indexing {
    uint texture_idx;
};

void main() {
    frag_color = vec4(texture(textures[texture_idx], uvs));
}
