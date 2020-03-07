#ifndef CAELUS_RENDERPASS_HPP
#define CAELUS_RENDERPASS_HPP

#include <engine/Forwards.hpp>

namespace caelus::core::api {
    [[nodiscard]] vk::RenderPass make_default_render_pass(const VulkanContext&);
} // namespace caelus::core::api

#endif //CAELUS_RENDERPASS_HPP
