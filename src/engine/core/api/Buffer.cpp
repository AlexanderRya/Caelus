#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/CommandBuffer.hpp>
#include <engine/core/api/VertexBuffer.hpp>
#include <engine/core/api/Device.hpp>
#include <engine/core/api/Buffer.hpp>

namespace caelus::core::api {
    Buffer vma_make_buffer(const usize size, const vk::BufferUsageFlags& usage, const VmaMemoryUsage memory_usage, const VmaAllocationCreateFlags alloc_flags, const api::VulkanContext& ctx) {
        vk::BufferCreateInfo buffer_create_info{}; {
            buffer_create_info.size = size;
            buffer_create_info.queueFamilyIndexCount = 1;
            buffer_create_info.pQueueFamilyIndices = &ctx.device.queue_family;
            buffer_create_info.usage = usage;
            buffer_create_info.sharingMode = vk::SharingMode::eExclusive;
        }

        VmaAllocationCreateInfo allocation_create_info{}; {
            allocation_create_info.flags = alloc_flags;
            allocation_create_info.requiredFlags = 0;
            allocation_create_info.preferredFlags = 0;
            allocation_create_info.memoryTypeBits = 0;
            allocation_create_info.pool = nullptr;
            allocation_create_info.pUserData = nullptr;
            allocation_create_info.usage = memory_usage;
        }

        Buffer buffer{};

        vmaCreateBuffer(
            ctx.allocator,
            reinterpret_cast<VkBufferCreateInfo*>(&buffer_create_info),
            &allocation_create_info,
            reinterpret_cast<VkBuffer*>(&buffer.handle),
            &buffer.allocation,
            nullptr);

        return buffer;
    }

    vk::Buffer make_buffer(const VulkanContext& ctx, const usize size, const vk::BufferUsageFlags usage) {
        vk::BufferCreateInfo buffer_create_info{}; {
            buffer_create_info.size = size;
            buffer_create_info.queueFamilyIndexCount = 1;
            buffer_create_info.pQueueFamilyIndices = &ctx.device.queue_family;
            buffer_create_info.usage = usage;
            buffer_create_info.sharingMode = vk::SharingMode::eExclusive;
        }

        return ctx.device.logical.createBuffer(buffer_create_info, nullptr, ctx.dispatcher);
    }

    vk::DeviceMemory allocate_memory(const VulkanContext& ctx, const vk::Buffer buffer, const vk::MemoryPropertyFlags flags) {
        const auto memory_requirements = ctx.device.logical.getBufferMemoryRequirements(buffer, ctx.dispatcher);

        vk::MemoryAllocateInfo memory_allocate_info{}; {
            memory_allocate_info.allocationSize = memory_requirements.size;
            memory_allocate_info.memoryTypeIndex = api::find_memory_type(
                ctx, memory_requirements.memoryTypeBits, flags);
        }

        auto memory = ctx.device.logical.allocateMemory(memory_allocate_info, nullptr, ctx.dispatcher);

        ctx.device.logical.bindBufferMemory(buffer, memory, 0, ctx.dispatcher);

        return memory;
    }

    vk::DeviceMemory allocate_memory(const VulkanContext& ctx, const vk::Image image, const vk::MemoryPropertyFlags flags) {
        const auto memory_requirements = ctx.device.logical.getImageMemoryRequirements(image, ctx.dispatcher);

        vk::MemoryAllocateInfo memory_allocate_info{}; {
            memory_allocate_info.allocationSize = memory_requirements.size;
            memory_allocate_info.memoryTypeIndex = api::find_memory_type(
                ctx, memory_requirements.memoryTypeBits, flags);
        }

        auto memory = ctx.device.logical.allocateMemory(memory_allocate_info, nullptr, ctx.dispatcher);

        ctx.device.logical.bindImageMemory(image, memory, 0, ctx.dispatcher);

        return memory;
    }

    void copy_buffer(const VulkanContext& ctx, const vk::Buffer src, vk::Buffer dst, const usize size) {
        auto command_buffer = begin_transient(ctx); {
            vk::BufferCopy region{}; {
                region.size = size;
                region.srcOffset = 0;
                region.dstOffset = 0;
            }

            command_buffer.copyBuffer(src, dst, region, ctx.dispatcher);

            end_transient(ctx, command_buffer);
        }
    }

    void copy_buffer_to_image(const VulkanContext& ctx, const vk::Buffer buffer, vk::Image image, const u32 width, const u32 height) {
        auto command_buffer = begin_transient(ctx); {
            vk::BufferImageCopy region{}; {
                region.bufferOffset = 0;
                region.bufferRowLength = 0;
                region.bufferImageHeight = 0;

                region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                region.imageSubresource.mipLevel = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount = 1;

                region.imageOffset = { { 0, 0, 0 } };
                region.imageExtent = { {
                    width,
                    height,
                    1
                } };
            }

            command_buffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region, ctx.dispatcher);

            end_transient(ctx, command_buffer);
        }
    }
} // namespace caelus::core::api