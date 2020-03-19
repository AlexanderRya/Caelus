#include <engine/core/components/Mesh.hpp>

namespace caelus::core::components {
    std::vector<Vertex> generate_triangle_geometry() {
        return { {
            { { 0.0f, 0.5f, 0.0f } },
            { { 0.5f, -0.5f, 0.0f } },
            { { -0.5f, -0.5f, 0.0f } }
        } };
    }

    std::vector<Vertex> generate_quad_geometry() {
        return { {
            { { 0.0f, 1.0f, 0.0f } },
            { { 1.0f, 0.0f, 0.0f } },
            { { 0.0f, 0.0f, 0.0f } },
            { { 0.0f, 1.0f, 0.0f } },
            { { 1.0f, 1.0f, 0.0f } },
            { { 1.0f, 0.0f, 0.0f } },
        } };
    }
} // namespace caelus::core::components