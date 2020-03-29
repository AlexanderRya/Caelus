#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/components/Camera.hpp>
#include <engine/core/Globals.hpp>
#include <engine/core/Window.hpp>
#include <engine/Logger.hpp>

#include <stdexcept>

namespace caelus::core {
    Window::Window(const i32 w, const i32 h, const char* title) {
        glfwSetErrorCallback([](const i32 code, const char* message) {
            std::cout << util::format(
                "[{}] [GLFW3] [Error: {}]: {}\n",
                util::get_current_timestamp(),
                code,
                message);
        });

        if (!glfwInit()) {
            throw std::runtime_error("Failed glfw init");
        }

        logger::info("glfwInit() success");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        width = w;
        height = h;

        if (!(window = glfwCreateWindow(width, height, title, nullptr, nullptr))) {
            throw std::runtime_error("Failed window creation");
        }

        glfwSetMouseButtonCallback(window, [](GLFWwindow* win, const int button, const int action, const int) {
            if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
                if (!captured) {
                    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }

                captured = true;
            } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
                if (captured) {
                    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }

                captured = false;
            }
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow*, const double xpos, const double ypos) {
            static double lastX = width / 2.0, lastY = height / 2.0;
            static bool first = true;

            if (captured) {
                if (first) {
                    lastX = xpos;
                    lastY = ypos;
                    first = false;
                }

                double xoffset = xpos - lastX;
                double yoffset = lastY - ypos;

                lastX = xpos;
                lastY = ypos;

                camera.process(xoffset, yoffset);
            }
        });


        logger::info("Window successfully created with size: ", width, "x", height);
    }

    vk::SurfaceKHR Window::create_surface(const api::VulkanContext& ctx) const {
        VkSurfaceKHR surface;

        glfwCreateWindowSurface(static_cast<VkInstance>(ctx.instance), window, nullptr, &surface);

        return surface;
    }

    void Window::poll_events() const {
        glfwPollEvents();
    }

    void Window::set_user_pointer(void* ptr) {
        glfwSetWindowUserPointer(window, ptr);
    }

    bool Window::should_close() const {
        return glfwWindowShouldClose(window);
    }

    void Window::close() const {
        glfwSetWindowShouldClose(window, true);
    }

    i32 Window::get_key(const i32 key) const {
        return glfwGetKey(window, key);
    }
} // namespace caelus::core