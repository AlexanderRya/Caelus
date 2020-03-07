#ifndef CAELUS_VERTEXBUFFER_HPP
#define CAELUS_VERTEXBUFFER_HPP

#include <engine/core/api/Buffer.hpp>
#include <engine/Forwards.hpp>
#include <engine/Types.hpp>

#include <vulkan/vulkan.hpp>

namespace caelus::core::api {
    struct VertexBuffer {
        Buffer buffer{};
    };

    [[nodiscard]] VertexBuffer make_vertex_buffer(const std::vector<Vertex>&, const api::VulkanContext&);
} // namespace caelus::core::api

#endif //CAELUS_VERTEXBUFFER_HPP
