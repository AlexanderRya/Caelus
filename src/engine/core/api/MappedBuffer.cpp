#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/MappedBuffer.hpp>
#include <engine/core/api/Buffer.hpp>
#include <engine/Constants.hpp>
#include <engine/Logger.hpp>

namespace caelus::core::api {
    void SingleMappedBuffer::create(const MappedBuffer::CreateInfo& info) {
        ctx = info.ctx;
        buffer_usage = info.buffer_usage;
        type_size = info.type_size;

        // Arbitrary number
        allocate(1);
    }

    void SingleMappedBuffer::allocate(const usize size) {
        current_size = size;

        buffer = vma_make_buffer(
            size * type_size,
            buffer_usage,
            VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_ONLY,
            VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT,
            *ctx);

        vmaMapMemory(ctx->allocator, buffer.allocation, &mapped);
    }

    void SingleMappedBuffer::write(const void* data, const usize size) {
        if (size > current_size) {
            deallocate();
            allocate(size);
        }

        std::memcpy(mapped, data, size * type_size);
        current_size = size;
    }

    void SingleMappedBuffer::deallocate() {
        vmaUnmapMemory(ctx->allocator, buffer.allocation);

        vmaDestroyBuffer(ctx->allocator, static_cast<VkBuffer>(buffer.handle), buffer.allocation);
    }

    vk::DescriptorBufferInfo SingleMappedBuffer::get_info() const {
        vk::DescriptorBufferInfo info{}; {
            info.buffer = buffer.handle;
            info.offset = static_cast<vk::DeviceSize>(0);
            info.range = current_size * type_size;
        }

        return info;
    }

    usize SingleMappedBuffer::size() const {
        return current_size;
    }

    void MappedBuffer::create(const CreateInfo& create_info) {
        mapped_buffers.reserve(meta::frames_in_flight);

        for (usize i = 0; i < meta::frames_in_flight; ++i) {
            mapped_buffers.emplace_back().create(create_info);
        }

        logger::info("Created mapped buffer with size (in bytes): ", create_info.type_size);
    }

    std::vector<vk::DescriptorBufferInfo> MappedBuffer::get_info() const {
        std::vector<vk::DescriptorBufferInfo> info{};
        info.reserve(meta::frames_in_flight);

        for (usize i = 0; i < meta::frames_in_flight; ++i) {
            info.emplace_back(mapped_buffers[i].get_info());
        }

        return info;
    }

    SingleMappedBuffer& MappedBuffer::operator [](const usize idx) {
        return mapped_buffers[idx];
    }

    vk::Buffer SingleMappedBuffer::handle() const {
        return buffer.handle;
    }
} // namespace caelus::core::api