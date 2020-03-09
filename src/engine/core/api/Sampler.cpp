#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/Sampler.hpp>
#include <vulkan/vulkan.hpp>

namespace caelus::core::api {
    vk::Sampler make_default_sampler(const VulkanContext& ctx) {
        vk::SamplerCreateInfo create_info{}; {
            create_info.magFilter = vk::Filter::eLinear;
            create_info.minFilter = vk::Filter::eLinear;
            create_info.addressModeU = vk::SamplerAddressMode::eRepeat;
            create_info.addressModeV = vk::SamplerAddressMode::eRepeat;
            create_info.addressModeW = vk::SamplerAddressMode::eRepeat;
            create_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
            create_info.anisotropyEnable = true;
            create_info.maxAnisotropy = 16;
            create_info.compareEnable = false;
            create_info.compareOp = {};
            create_info.mipmapMode = {};
            create_info.unnormalizedCoordinates = false;
        }

        return ctx.device.logical.createSampler(create_info, nullptr, ctx.dispatcher);
    }
} // namespace caelus::core::api