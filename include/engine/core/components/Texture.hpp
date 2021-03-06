#ifndef CAELUS_TEXTURE_HPP
#define CAELUS_TEXTURE_HPP

#include <engine/core/api/Image.hpp>
#include <engine/Constants.hpp>
#include <engine/Forwards.hpp>
#include <engine/Types.hpp>

#include <filesystem>

namespace caelus::core::components {
    class Texture {
         api::Image image;
         vk::ImageView image_view;

         const vk::Sampler sampler;
         const api::VulkanContext& ctx;
    public:
        explicit Texture(const api::VulkanContext&, const vk::Sampler);

        void load(const std::filesystem::path&);

        [[nodiscard]] vk::DescriptorImageInfo info() const;
    };
} // namespace caelus::core::components

#endif //CAELUS_TEXTURE_HPP
