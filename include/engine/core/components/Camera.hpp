#ifndef CAELUS_CAMERA_HPP
#define CAELUS_CAMERA_HPP

#include <engine/Forwards.hpp>
#include <engine/Types.hpp>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace caelus::core::components {
    class Camera {
        static constexpr f32 speed = 2.f;
        static constexpr f32 sensitivity = 8.0e-2f;

        f64 yaw;
        f64 pitch;

        glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cam_right = glm::vec3();
        constexpr static glm::vec3 cam_wup = glm::vec3(0.0f, 1.0f, 0.0f);
    public:
        Camera() noexcept;
        void move(const Window&);
        [[nodiscard]] glm::mat4 view() const;
        void process(double xoffset, double yoffset);
        void update();
    };
} // namespace caelus::core::components

#endif //CAELUS_CAMERA_HPP
