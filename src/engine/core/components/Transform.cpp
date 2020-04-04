#include <engine/core/components/Transform.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace caelus::core::components {
    glm::mat4 Transform::model() const {
        glm::mat4 mat(1.0f);

        mat = glm::scale(mat, scale);
        mat = glm::rotate(mat, rotation, { 0.0f, 0.0f, 1.0f });
        mat = glm::translate(mat, position);

        return mat;
    }
} // namespace caelus::core::components