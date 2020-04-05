#ifndef CAELUS_SWAPCHAIN_HPP
#define CAELUS_SWAPCHAIN_HPP

#include <engine/Forwards.hpp>

namespace caelus::core::api {
    [[nodiscard]] Swapchain make_swapchain(const VulkanContext&);
} // namespace caelus::core::api

#endif //CAELUS_SWAPCHAIN_HPP
