#ifndef CAELUS_FRAMEBUFFER_HPP
#define CAELUS_FRAMEBUFFER_HPP

#include <engine/Forwards.hpp>

#include <vector>

namespace caelus::core::api {
    [[nodiscard]] std::vector<vk::Framebuffer> make_default_framebuffers(const VulkanContext&);
} // namespace caelus::core::api

#endif //CAELUS_FRAMEBUFFER_HPP
