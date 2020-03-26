#include <engine/core/components/Transform.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace caelus::core::components {
    glm::mat4 Transform::Instance::model() const {
        glm::mat4 mat(1.0f);

        mat = glm::scale(mat, scale);
        mat = glm::rotate(mat, rotation, { 0.0f, 0.0f, 1.0f });
        mat = glm::translate(mat, position);

        return mat;
    }

    std::vector<glm::mat4> Transform::models() {
        std::vector<glm::mat4> models{};
        models.reserve(instances.size());

        for (const auto& instance : instances) {
            models.emplace_back(instance.model());
        }

        return models;
    }
} // namespace caelus::core::components