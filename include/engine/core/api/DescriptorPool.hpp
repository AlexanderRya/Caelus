#ifndef CAELUS_DESCRIPTORPOOL_HPP
#define CAELUS_DESCRIPTORPOOL_HPP

#include <engine/Forwards.hpp>

namespace caelus::core::api {
    [[nodiscard]] vk::DescriptorPool make_descriptor_pool(const VulkanContext&);
} // namespace caelus::core::api

#endif //CAELUS_DESCRIPTORPOOL_HPP
