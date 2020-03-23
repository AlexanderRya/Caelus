#ifndef CAELUS_TRANSFORM_HPP
#define CAELUS_TRANSFORM_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <vector>

namespace caelus::core::components {
    namespace detail {
        struct alignas(16) InstanceGLSL {
            glm::mat4 model;
            glm::vec2 uvs;
        };
    } // namespace caelus::core::components::detail

    struct Transform {
        struct Instance {
            glm::vec3 position;
            glm::vec3 scale;
            float rotation;
        };

        std::vector<Instance> instances;
    };
} // namespace caelus::core::components

#endif //CAELUS_TRANSFORM_HPP
