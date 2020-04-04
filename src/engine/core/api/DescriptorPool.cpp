#include <engine/core/api/DescriptorPool.hpp>
#include <engine/core/api/VulkanContext.hpp>
#include <engine/Logger.hpp>
#include <engine/Types.hpp>

#include <vulkan/vulkan.hpp>

namespace caelus::core::api {
    vk::DescriptorPool make_descriptor_pool(const VulkanContext& ctx) {
        std::array<vk::DescriptorPoolSize, 3> descriptor_pool_sizes{ {
            { vk::DescriptorType::eCombinedImageSampler, 1000 },
            { vk::DescriptorType::eUniformBuffer, 100000 },
            { vk::DescriptorType::eStorageBuffer, 100000 },
        } };

        vk::DescriptorPoolCreateInfo descriptor_pool_create_info{}; {
            descriptor_pool_create_info.poolSizeCount = descriptor_pool_sizes.size();
            descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();
            descriptor_pool_create_info.maxSets = descriptor_pool_sizes.size() * 100000;
        }

        auto pool = ctx.device.logical.createDescriptorPool(descriptor_pool_create_info, nullptr, ctx.dispatcher);

        logger::info("Descriptor pool successfully created, sizes:");
        for (const auto& [type, count] : descriptor_pool_sizes) {
            logger::info("Type: ", vk::to_string(type), ", Count: ", count);
        }

        return pool;
    }
} // namespace caelus::core::api