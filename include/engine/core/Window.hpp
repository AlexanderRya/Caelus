#ifndef CAELUS_WINDOW_HPP
#define CAELUS_WINDOW_HPP

#include <engine/Forwards.hpp>
#include <engine/Types.hpp>

namespace caelus::core {
    class Window {
        GLFWwindow* window{};
    public:
        static inline i32 width{}, height{};
        static inline bool captured = false;

        Window(const i32, const i32, const char*);

        Window(const Window&) = delete;
        Window& operator =(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator =(Window&&) = delete;

        [[nodiscard]] vk::SurfaceKHR create_surface(const api::VulkanContext&) const;

        void poll_events() const;
        void close() const;
        void set_user_pointer(void*);
        [[nodiscard]] bool should_close() const;
        [[nodiscard]] i32 get_key(const i32) const;
    };
} // namespace caelus::core

#endif //CAELUS_WINDOW_HPP
