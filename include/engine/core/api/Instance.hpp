#ifndef CAELUS_INSTANCE_HPP
#define CAELUS_INSTANCE_HPP

#include <engine/Forwards.hpp>

namespace caelus::core::api {
    [[nodiscard]] vk::Instance make_instance(const VulkanContext&);
    [[nodiscard]] vk::DebugUtilsMessengerEXT install_validation_layers(const VulkanContext&);
} // namespace caelus::core::api

#endif //CAELUS_INSTANCE_HPP
