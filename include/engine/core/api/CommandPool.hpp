#ifndef CAELUS_COMMANDPOOL_HPP
#define CAELUS_COMMANDPOOL_HPP

#include <engine/Forwards.hpp>

namespace caelus::core::api {
    [[nodiscard]] vk::CommandPool make_command_pool(const VulkanContext&);
    [[nodiscard]] vk::CommandPool make_transient_pool(const VulkanContext&);
} // namespace caelus::core::api

#endif //CAELUS_COMMANDPOOL_HPP
