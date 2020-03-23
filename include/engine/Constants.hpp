#ifndef CAELUS_CONSTANTS_HPP
#define CAELUS_CONSTANTS_HPP

#include <engine/Types.hpp>

namespace caelus::meta {
    constexpr inline usize frames_in_flight = 2;

    enum class PipelineBinding : u32 {
        Camera = 0,
        Instance = 1,
        DefaultSampler = 2
    };

    enum class PipelineType {
        MeshGeneric
    };

    enum class PipelineLayoutType {
        MeshGeneric
    };

    enum class SamplerType {
        Default
    };
} // namespace caelus::meta

#endif //CAELUS_CONSTANTS_HPP
