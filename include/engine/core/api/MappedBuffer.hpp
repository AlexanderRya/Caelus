#ifndef CAELUS_MAPPEDBUFFER_HPP
#define CAELUS_MAPPEDBUFFER_HPP

#include <engine/core/api/Buffer.hpp>
#include <engine/Forwards.hpp>
#include <engine/Types.hpp>

#include <vulkan/vulkan.hpp>

namespace caelus::core::api {
    class MappedBuffer {
        std::vector<SingleMappedBuffer> mapped_buffers{};
    public:
        struct CreateInfo {
            const api::VulkanContext* ctx;
            usize type_size;
            vk::BufferUsageFlags buffer_usage;
        };

        MappedBuffer() = default;

        void create(const CreateInfo&);

        [[nodiscard]] std::vector<vk::DescriptorBufferInfo> get_info() const;

        [[nodiscard]] SingleMappedBuffer& operator [](const usize);
    };

    class SingleMappedBuffer {
        const api::VulkanContext* ctx{};

        vk::BufferUsageFlags buffer_usage{};
        api::Buffer buffer{};

        void* mapped{};

        usize current_size{};
        usize type_size{};
    public:
        SingleMappedBuffer() = default;

        void create(const MappedBuffer::CreateInfo&);

        void allocate(const usize);
        void write(const void*, const usize);
        void deallocate();

        [[nodiscard]] vk::DescriptorBufferInfo get_info() const;
        [[nodiscard]] usize size() const;
    };
} // namespace caelus::core::api

#endif //CAELUS_MAPPEDBUFFER_HPP
