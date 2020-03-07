#include <engine/core/api/DescriptorPool.hpp>
#include <engine/core/api/VulkanContext.hpp>
#include <engine/Logger.hpp>
#include <engine/Types.hpp>

#include <vulkan/vulkan.hpp>

namespace caelus::core::api {
    vk::DescriptorPool make_descriptor_pool(const VulkanContext& ctx) {
        std::array<vk::DescriptorPoolSize, 11> descriptor_pool_sizes{ {
            { vk::DescriptorType::eSampler, 1000 },
            { vk::DescriptorType::eCombinedImageSampler, 1000 },
            { vk::DescriptorType::eSampledImage, 1000 },
            { vk::DescriptorType::eStorageImage, 1000 },
            { vk::DescriptorType::eUniformTexelBuffer, 1000 },
            { vk::DescriptorType::eStorageTexelBuffer, 1000 },
            { vk::DescriptorType::eUniformBuffer, 1000 },
            { vk::DescriptorType::eStorageBuffer, 1000 },
            { vk::DescriptorType::eUniformBufferDynamic, 1000 },
            { vk::DescriptorType::eStorageBufferDynamic, 1000 },
            { vk::DescriptorType::eInputAttachment, 1000 }
        } };

        vk::DescriptorPoolCreateInfo descriptor_pool_create_info{}; {
            descriptor_pool_create_info.poolSizeCount = descriptor_pool_sizes.size();
            descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();
            descriptor_pool_create_info.maxSets = descriptor_pool_sizes.size() * 1000;
        }

        auto pool = ctx.device.logical.createDescriptorPool(descriptor_pool_create_info, nullptr, ctx.dispatcher);

        logger::info("Descriptor pool successfully created, sizes:");
        for (const auto& [type, count] : descriptor_pool_sizes) {
            logger::info("Type: ", vk::to_string(type), ", Count: ", count);
        }

        return pool;
    }
} // namespace caelus::core::api