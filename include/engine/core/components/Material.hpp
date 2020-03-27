#ifndef CAELUS_MATERIAL_HPP
#define CAELUS_MATERIAL_HPP

#include <engine/Types.hpp>

#include <glm/vec4.hpp>

namespace caelus::core::components {
    struct Material {
        struct alignas(16) Instance {
            glm::vec4 color;
            i32 texture_idx;
        };

        std::vector<Instance> materials;
    };
} // namespace caelus::core::components

#endif //CAELUS_MATERIAL_HPP
