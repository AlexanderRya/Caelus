#ifndef CAELUS_CONSTANTS_HPP
#define CAELUS_CONSTANTS_HPP

namespace caelus::meta {
    constexpr static inline usize frames_in_flight = 2;

    enum class PipelineBinding : u32 {
        Camera = 0,
        Instance = 1,
        Color = 2,
        DefaultSampler = 3
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
