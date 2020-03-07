#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/Swapchain.hpp>
#include <engine/core/api/Image.hpp>
#include <engine/core/Window.hpp>
#include <engine/Logger.hpp>
#include <engine/Types.hpp>

#include <vulkan/vulkan.hpp>

namespace caelus::core::api {
    [[nodiscard]] static inline u32 get_image_count(const vk::SurfaceCapabilitiesKHR& capabilities) {
        auto count = capabilities.minImageCount + 1;

        if (capabilities.maxImageCount > 0 && count > capabilities.maxImageCount) {
            count = capabilities.maxImageCount;
        }

        logger::info("Swapchain details: image count: ", count);

        return count;
    }

    [[nodiscard]] static inline vk::Extent2D get_extent(const Window* window, const vk::SurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {

            return capabilities.currentExtent;
        } else {
            vk::Extent2D actual_extent{ static_cast<u32>(window->width), static_cast<u32>(window->height) };

            actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actual_extent;
        }
    }

    [[nodiscard]] static inline vk::SurfaceFormatKHR get_format(const VulkanContext& ctx) {
        auto surface_formats = ctx.device.physical.getSurfaceFormatsKHR(ctx.surface, {}, ctx.dispatcher);

        vk::SurfaceFormatKHR format = surface_formats[0];

        for (const auto& each : surface_formats) {
            if (each.format == vk::Format::eB8G8R8A8Srgb &&
                each.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {

                logger::info("Swapchain details: format: vk::Format::", vk::to_string(each.format));
                logger::info("Swapchain details: color space: vk::ColorSpaceKHR::", vk::to_string(each.colorSpace));
                return each;
            }
        }

        logger::info("Swapchain details: non-preferred format: vk::Format::", vk::to_string(format.format));
        logger::info("Swapchain details: non-preferred color space: vk::ColorSpaceKHR::", vk::to_string(format.colorSpace));

        return format;
    }

    [[nodiscard]] static inline vk::PresentModeKHR get_present_mode(const VulkanContext& ctx) {
        for (const auto& mode : ctx.device.physical.getSurfacePresentModesKHR(ctx.surface, {}, ctx.dispatcher)) {
            if (mode == vk::PresentModeKHR::eImmediate) {
                logger::info("Swapchain details: present mode: vk::PresentModeKHR::", vk::to_string(mode));
                return mode;
            }
        }

        logger::warning("Swapchain details: non-preferreds present mode: vk::PresentModeKHR::", vk::to_string(vk::PresentModeKHR::eFifo));

        return vk::PresentModeKHR::eFifo;
    }

    static inline void get_swapchain(const VulkanContext& ctx, Swapchain& swapchain) {
        vk::SwapchainCreateInfoKHR swapchain_create_info{}; {
            swapchain_create_info.surface = ctx.surface;
            swapchain_create_info.minImageCount = swapchain.image_count;
            swapchain_create_info.imageFormat = swapchain.format.format;
            swapchain_create_info.imageColorSpace = swapchain.format.colorSpace;
            swapchain_create_info.imageExtent = swapchain.extent;
            swapchain_create_info.preTransform = swapchain.surface_transform;
            swapchain_create_info.imageArrayLayers = 1;
            swapchain_create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
            swapchain_create_info.imageSharingMode = vk::SharingMode::eExclusive;
            swapchain_create_info.queueFamilyIndexCount = 1;
            swapchain_create_info.pQueueFamilyIndices = &ctx.device.queue_family;
            swapchain_create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
            swapchain_create_info.presentMode = swapchain.present_mode;
            swapchain_create_info.clipped = true;
            swapchain_create_info.oldSwapchain = nullptr;
        }

        swapchain.handle = ctx.device.logical.createSwapchainKHR(swapchain_create_info, nullptr, ctx.dispatcher);

        logger::info("Swapchain successfully created");
    }

    static inline void create_images(const VulkanContext& ctx, Swapchain& swapchain) {
        swapchain.images = ctx.device.logical.getSwapchainImagesKHR(swapchain.handle, ctx.dispatcher);

        swapchain.image_views.reserve(swapchain.image_count);

        for (const auto& image : swapchain.images) {
            swapchain.image_views.emplace_back(api::make_image_view(ctx, image, swapchain.format.format, vk::ImageAspectFlagBits::eColor));
        }

        logger::info("Swapchain images successfully created");
    }

    static inline void make_depth_image(const VulkanContext& ctx, Swapchain& swapchain) {
        api::Image::CreateInfo create_info{}; {
            create_info.ctx = &ctx;
            create_info.format = vk::Format::eD32SfloatS8Uint;
            create_info.tiling = vk::ImageTiling::eOptimal;
            create_info.usage_flags = vk::ImageUsageFlagBits::eDepthStencilAttachment;
            create_info.memory_usage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
            create_info.height = swapchain.extent.height;
            create_info.width = swapchain.extent.width;
        }

        swapchain.depth_image = api::make_image(create_info);
        swapchain.depth_view = api::make_image_view(ctx, swapchain.depth_image.handle, vk::Format::eD32SfloatS8Uint, vk::ImageAspectFlagBits::eDepth);

        api::transition_image_layout(ctx, swapchain.depth_image.handle, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }

    Swapchain make_swapchain(const Window* window, const VulkanContext& ctx) {
        auto capabilities = ctx.device.physical.getSurfaceCapabilitiesKHR(ctx.surface, ctx.dispatcher);

        Swapchain swapchain{};

        swapchain.image_count = get_image_count(capabilities);
        swapchain.extent = get_extent(window, capabilities);
        swapchain.format = get_format(ctx);
        swapchain.present_mode = get_present_mode(ctx);

        swapchain.surface_transform = capabilities.currentTransform;

        get_swapchain(ctx, swapchain);
        create_images(ctx, swapchain);

        make_depth_image(ctx, swapchain);

        return swapchain;
    }
} // namespace caelus::core::api