#ifndef CAELUS_MESH_HPP
#define CAELUS_MESH_HPP

#include <engine/core/api/DescriptorSet.hpp>
#include <engine/core/api/MappedBuffer.hpp>
#include <engine/Types.hpp>

#include <vector>

namespace caelus::core::components {
    struct Mesh {
        usize vertex_buffer_idx{};
        u64 vertex_count{};
    };

    [[nodiscard]] std::vector<Vertex> generate_triangle_geometry();
    [[nodiscard]] std::vector<Vertex> generate_quad_geometry();
    [[nodiscard]] std::vector<Vertex> generate_cube_geometry();
} // namespace caelus::core::components

#endif //CAELUS_MESH_HPP
