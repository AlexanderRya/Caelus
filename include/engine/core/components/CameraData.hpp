#ifndef CAELUS_CAMERADATA_HPP
#define CAELUS_CAMERADATA_HPP

#include <engine/core/api/MappedBuffer.hpp>

namespace caelus::core::components {
    struct CameraData {
        api::MappedBuffer buffer{};
    };
} // namespace caelus::core::components

#endif //CAELUS_CAMERADATA_HPP
