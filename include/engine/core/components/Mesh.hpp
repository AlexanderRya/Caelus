#ifndef CAELUS_MESH_HPP
#define CAELUS_MESH_HPP

#include <engine/Types.hpp>

#include <vector>

namespace caelus::core::components {
    [[nodiscard]] std::vector<Vertex> generate_triangle_geometry();
    [[nodiscard]] std::vector<Vertex> generate_quad_geometry();
}

#endif //CAELUS_MESH_HPP
