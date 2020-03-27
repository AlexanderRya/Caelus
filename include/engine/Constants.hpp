#ifndef CAELUS_CONSTANTS_HPP
#define CAELUS_CONSTANTS_HPP

#include <engine/Types.hpp>

namespace caelus::meta {
    constexpr inline usize frames_in_flight = 2;

    enum class PipelineBinding : u32 {
        eCamera = 0,
        eInstance = 1,
        eMaterial = 2,
        eTexture = 3
    };

    enum class PipelineType {
        eMeshGeneric
    };

    enum class PipelineLayoutType {
        eMeshGeneric
    };

    enum class SamplerType {
        eDefault
    };
} // namespace caelus::meta

#endif //CAELUS_CONSTANTS_HPP
