#ifndef CAELUS_DEVICE_HPP
#define CAELUS_DEVICE_HPP

#include <engine/Forwards.hpp>

namespace caelus::core::api {
    [[nodiscard]] Device make_device(const VulkanContext&);
    [[nodiscard]] u64 find_memory_type(const VulkanContext&, const u32, const vk::MemoryPropertyFlags&);
} // namespace caelus::core::api

#endif //CAELUS_DEVICE_HPP
