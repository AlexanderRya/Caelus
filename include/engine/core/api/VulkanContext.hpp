#ifndef CAELUS_VULKANCONTEXT_HPP
#define CAELUS_VULKANCONTEXT_HPP

#include <engine/core/api/Image.hpp>
#include <engine/Forwards.hpp>
#include <engine/Types.hpp>

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

#include <vector>

namespace caelus::core::api {
    struct Device {
        vk::PhysicalDevice physical{};
        vk::Device logical{};
        vk::Queue queue{};
        u32 queue_family{};
    };

    struct Swapchain {
        vk::SwapchainKHR handle{};
        u32 image_count{};

        vk::Extent2D extent{};
        vk::SurfaceFormatKHR format{};
        vk::PresentModeKHR present_mode{};
        vk::SurfaceTransformFlagBitsKHR surface_transform{};

        std::vector<vk::Image> images{};
        std::vector<vk::ImageView> image_views{};

        api::Image depth_image{};
        vk::ImageView depth_view{};
    };

    struct VulkanContext {
        vk::DispatchLoaderDynamic dispatcher{};
        VmaVulkanFunctions vma_dispatcher{};
        vk::Instance instance{};
        VmaAllocator allocator{};
        vk::DebugUtilsMessengerEXT validation{};
        vk::SurfaceKHR surface{};
        Device device{};
        Swapchain swapchain{};
        vk::CommandPool command_pool{};
        vk::CommandPool transient_pool{};
        vk::DescriptorPool descriptor_pool{};
        vk::RenderPass default_render_pass{};
        std::vector<vk::Framebuffer> default_framebuffers{};
    };

    [[nodiscard]] VulkanContext make_vulkan_context(Window*);
} // namespace caelus::core::api

#endif //CAELUS_VULKANCONTEXT_HPP
