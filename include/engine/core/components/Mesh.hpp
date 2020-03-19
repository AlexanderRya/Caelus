#ifndef CAELUS_MESH_HPP
#define CAELUS_MESH_HPP

#include <engine/core/api/DescriptorSet.hpp>
#include <engine/core/api/MappedBuffer.hpp>
#include <engine/Types.hpp>

#include <vector>

namespace caelus::core::components {
    struct Mesh {
        usize vertex_buffer_idx;
        u64 vertex_count;

        api::DescriptorSet descriptor_set;
        api::MappedBuffer instance_buffer;
    };

    [[nodiscard]] std::vector<Vertex> generate_triangle_geometry();
    [[nodiscard]] std::vector<Vertex> generate_quad_geometry();
}

#endif //CAELUS_MESH_HPP
