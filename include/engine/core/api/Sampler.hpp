#ifndef CAELUS_SAMPLER_HPP
#define CAELUS_SAMPLER_HPP

#include <engine/Forwards.hpp>
#include <engine/Types.hpp>

namespace caelus::core::api {
    [[nodiscard]] vk::Sampler make_default_sampler(const VulkanContext&);
}

#endif //CAELUS_SAMPLER_HPP
