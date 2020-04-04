#ifndef CAELUS_TRANSFORM_HPP
#define CAELUS_TRANSFORM_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <vector>

namespace caelus::core::components {
    struct Transform {
        glm::vec3 position;
        glm::vec3 scale;
        float rotation;

        [[nodiscard]] glm::mat4 model() const;
    };
} // namespace caelus::core::components

#endif //CAELUS_TRANSFORM_HPP
