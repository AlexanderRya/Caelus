#ifndef CAELUS_MESHRENDERER_HPP
#define CAELUS_MESHRENDERER_HPP

#include <engine/core/api/DescriptorSet.hpp>
#include <engine/core/api/MappedBuffer.hpp>

namespace caelus::core::components {
    struct MeshRenderer {
        api::DescriptorSet descriptor_set{};
        api::MappedBuffer instance_buffer{};
    };
} // namespace caelus::core::components

#endif //CAELUS_MESHRENDERER_HPP
