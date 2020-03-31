#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/CommandBuffer.hpp>
#include <engine/core/api/Buffer.hpp>
#include <engine/core/api/Image.hpp>

namespace caelus::core::api {
    Image make_image(const Image::CreateInfo& info) {
        vk::ImageCreateInfo image_info{}; {
            image_info.imageType = vk::ImageType::e2D;
            image_info.extent = { {
                static_cast<u32>(info.width),
                static_cast<u32>(info.height),
                1
            } };
            image_info.mipLevels = 1;
            image_info.arrayLayers = 1;
            image_info.format = info.format;
            image_info.tiling = info.tiling;
            image_info.initialLayout = vk::ImageLayout::eUndefined;
            image_info.usage = info.usage_flags;
            image_info.samples = vk::SampleCountFlagBits::e1;
            image_info.sharingMode = vk::SharingMode::eExclusive;
        }

        VmaAllocationCreateInfo allocation_create_info{}; {
            allocation_create_info.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT;
            allocation_create_info.requiredFlags = 0;
            allocation_create_info.preferredFlags = 0;
            allocation_create_info.memoryTypeBits = 0;
            allocation_create_info.pool = nullptr;
            allocation_create_info.pUserData = nullptr;
            allocation_create_info.usage = info.memory_usage;
        }

        Image image{};

        vmaCreateImage(
            info.ctx->allocator,
            &static_cast<VkImageCreateInfo&>(image_info),
            &allocation_create_info,
            reinterpret_cast<VkImage*>(&image.handle),
            &image.allocation,
            nullptr);

        return image;
    }

    vk::ImageView make_image_view(const VulkanContext& ctx, const vk::Image image, const vk::Format format, const vk::ImageAspectFlags aspect) {
        vk::ImageViewCreateInfo image_view_create_info{}; {
            image_view_create_info.image = image;
            image_view_create_info.format = format;
            image_view_create_info.components.r = vk::ComponentSwizzle::eIdentity;
            image_view_create_info.components.g = vk::ComponentSwizzle::eIdentity;
            image_view_create_info.components.b = vk::ComponentSwizzle::eIdentity;
            image_view_create_info.components.a = vk::ComponentSwizzle::eIdentity;
            image_view_create_info.viewType = vk::ImageViewType::e2D;
            image_view_create_info.subresourceRange.aspectMask = aspect;
            image_view_create_info.subresourceRange.baseMipLevel = 0;
            image_view_create_info.subresourceRange.levelCount = 1;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount = 1;
        }

        return ctx.device.logical.createImageView(image_view_create_info, nullptr, ctx.dispatcher);
    }

    void transition_image_layout(const VulkanContext& ctx, vk::Image image, const vk::ImageLayout old_layout, const vk::ImageLayout new_layout) {
        auto command_buffer = begin_transient(ctx); {
            vk::ImageMemoryBarrier image_memory_barrier{}; {
                image_memory_barrier.image = image;

                image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                image_memory_barrier.oldLayout = old_layout;
                image_memory_barrier.newLayout = new_layout;

                image_memory_barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
                image_memory_barrier.subresourceRange.baseMipLevel = 0;
                image_memory_barrier.subresourceRange.levelCount = 1;
                image_memory_barrier.subresourceRange.baseArrayLayer = 0;
                image_memory_barrier.subresourceRange.layerCount = 1;

                image_memory_barrier.srcAccessMask = {};
                image_memory_barrier.dstAccessMask = {};
            }

            vk::PipelineStageFlags source_stage{};
            vk::PipelineStageFlags destination_stage{};

            if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal) {
                image_memory_barrier.srcAccessMask = {};
                image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

                source_stage = vk::PipelineStageFlagBits::eTopOfPipe;
                destination_stage = vk::PipelineStageFlagBits::eTransfer;
            } else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal) {
                image_memory_barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
                image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

                source_stage = vk::PipelineStageFlagBits::eTransfer;
                destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
            } else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
                image_memory_barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

                image_memory_barrier.srcAccessMask = {};
                image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

                source_stage = vk::PipelineStageFlagBits::eTopOfPipe;
                destination_stage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
            } else {
                throw std::invalid_argument("Unsupported layout transition");
            }

            command_buffer.pipelineBarrier(
                source_stage,
                destination_stage,
                vk::DependencyFlags{},
                nullptr,
                nullptr,
                image_memory_barrier,
                ctx.dispatcher);

            end_transient(ctx, command_buffer);
        }
    }
} // namespace caelus::core::api