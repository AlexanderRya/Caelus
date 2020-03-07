#ifndef CAELUS_BUFFER_HPP
#define CAELUS_BUFFER_HPP

#include <engine/Forwards.hpp>
#include <engine/Types.hpp>

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace caelus::core::api {
    struct Buffer {
        vk::Buffer handle{};
        VmaAllocation allocation{};
    };

    [[nodiscard]] Buffer vma_make_buffer(const usize, const vk::BufferUsageFlags&, const VmaMemoryUsage, const VmaAllocationCreateFlags, const VulkanContext&);
    [[nodiscard]] vk::Buffer make_buffer(const VulkanContext&, const usize, const vk::BufferUsageFlags);
    [[nodiscard]] vk::DeviceMemory allocate_memory(const VulkanContext&, const vk::Buffer, const vk::MemoryPropertyFlags);
    [[nodiscard]] vk::DeviceMemory allocate_memory(const VulkanContext&, const vk::Image, const vk::MemoryPropertyFlags);
    void copy_buffer(const VulkanContext&, const vk::Buffer, vk::Buffer, const usize);
    void copy_buffer_to_image(const VulkanContext&, const vk::Buffer, vk::Image, const u32, const u32);
} // namespace caelus::core::api

#endif //CAELUS_BUFFER_HPP
