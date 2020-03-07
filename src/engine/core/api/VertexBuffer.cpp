#include <engine/core/api/VertexBuffer.hpp>
#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/Device.hpp>
#include <engine/core/api/Buffer.hpp>
#include <engine/Logger.hpp>

namespace caelus::core::api {
    VertexBuffer make_vertex_buffer(const std::vector<Vertex>& vertices, const api::VulkanContext& ctx) {
        Buffer temp_buffer;
        // Allocate staging buffer
        temp_buffer = vma_make_buffer(
            vertices.size() * sizeof(Vertex),
            vk::BufferUsageFlagBits::eTransferSrc,
            VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_ONLY,
            VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT,
            ctx);

        void* mapped{};
        vmaMapMemory(ctx.allocator, temp_buffer.allocation, &mapped);
        std::memcpy(mapped, vertices.data(), sizeof(Vertex) * vertices.size());
        vmaUnmapMemory(ctx.allocator, temp_buffer.allocation);

        VertexBuffer vertex_buffer;
        // Allocate device local buffer
        vertex_buffer.buffer = vma_make_buffer(
            vertices.size() * sizeof(Vertex),
            vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
            VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY,
            VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT,
            ctx);

        // Copy to device local
        api::copy_buffer(ctx, temp_buffer.handle, vertex_buffer.buffer.handle, vertices.size() * sizeof(Vertex));

        vmaDestroyBuffer(ctx.allocator, temp_buffer.handle, temp_buffer.allocation);

        logger::info("Allocated vertex buffer with size (in bytes): ", vertices.size() * sizeof(Vertex));

        return vertex_buffer;
    }
} // namespace caelus::core::api