#ifndef CAELUS_COMMANDBUFFER_HPP
#define CAELUS_COMMANDBUFFER_HPP

#include <engine/Forwards.hpp>

#include <vector>

namespace caelus::core::api {
    [[nodiscard]] std::vector<vk::CommandBuffer> make_rendering_command_buffers(const VulkanContext&);
    [[nodiscard]] vk::CommandBuffer begin_transient(const VulkanContext&);
    void end_transient(const VulkanContext&, const vk::CommandBuffer);
} // namespace caelus::core::api

#endif //CAELUS_COMMANDBUFFER_HPP
