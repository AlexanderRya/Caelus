#include <engine/core/components/Texture.hpp>
#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/Buffer.hpp>
#include <engine/Constants.hpp>
#include <engine/Logger.hpp>

#include <stb_image.h>

#include <fstream>

namespace caelus::core::components {
    Texture::Texture(const api::VulkanContext& ctx, const vk::Sampler sampler)
        : sampler(sampler),
          ctx(ctx) {
        stbi_set_flip_vertically_on_load(true);
    }

    void Texture::load(const std::filesystem::path& path) {
        i32 width{}, height{}, channels = 4;

        if (!std::ifstream(path).is_open()) {
            throw std::runtime_error("File not found error at: " + path.generic_string());
        }

        auto data = stbi_load(path.generic_string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
        logger::info("Loading texture: " + path.generic_string());

        if (!data) {
            throw std::runtime_error("Failed to load texture!");
        }

        usize texture_size = width * height * 4;

        auto staging = api::vma_make_buffer(
            texture_size,
            vk::BufferUsageFlagBits::eTransferSrc,
            VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_ONLY,
            VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT,
            ctx);

        void* mapped{};
        vmaMapMemory(ctx.allocator, staging.allocation, &mapped);
        std::memcpy(mapped, data, texture_size);
        vmaUnmapMemory(ctx.allocator, staging.allocation);

        stbi_image_free(data);

        api::Image::CreateInfo create_info{}; {
            create_info.width = width;
            create_info.height = height;
            create_info.memory_usage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
            create_info.usage_flags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
            create_info.format = vk::Format::eR8G8B8A8Srgb;
            create_info.tiling = vk::ImageTiling::eOptimal;
            create_info.ctx = &ctx;
        }

        image = api::make_image(create_info);

        api::transition_image_layout(
            ctx, image.handle,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        api::copy_buffer_to_image(ctx, staging.handle, image.handle, width, height);
        api::transition_image_layout(
            ctx, image.handle,
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        image_view = api::make_image_view(ctx, image.handle, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);

        logger::info("Successfully loaded texture, "
                     "width: ", width,
                     ", height: ", height,
                     ", channels: ", channels);
    }

    vk::DescriptorImageInfo Texture::get_info() const {
        vk::DescriptorImageInfo info{}; {
            info.sampler = sampler;
            info.imageView = image_view;
            info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }

        return info;
    }
} // namespace caelus::core::components