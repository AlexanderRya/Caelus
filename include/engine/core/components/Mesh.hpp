#ifndef CAELUS_MESH_HPP
#define CAELUS_MESH_HPP

#include <engine/Types.hpp>

#include <vector>

namespace caelus::core::components {
    struct Mesh {
        u64 vertex_buffer_id;
        u64 vertex_count;
    };

    [[nodiscard]] std::vector<Vertex> generate_triangle_geometry();
    [[nodiscard]] std::vector<Vertex> generate_quad_geometry();
}

#endif //CAELUS_MESH_HPP
