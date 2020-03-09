#include <engine/core/components/Texture.hpp>
#include <engine/core/components/Camera.hpp>
#include <engine/core/components/Mesh.hpp>
#include <engine/core/api/Sampler.hpp>
#include <engine/core/Application.hpp>
#include <engine/core/Globals.hpp>

#include <entt/entt.hpp>

#include <iostream>
#include <fstream>

namespace caelus::core {
    Application::Application()
    : window(1280, 720, "Caelus"),
      context(api::make_vulkan_context(&window)),
      renderer(context) {}

    void Application::run() {
        entt::registry registry;

        auto triangle = registry.create(); {
            registry.assign<components::Mesh>(triangle, components::Mesh{ 0, 3 });
        }

        renderer.build(registry);
        
        while (!window.should_close()) {
            f32 frame_time = glfwGetTime();
            delta_time = frame_time - last_frame;
            last_frame = frame_time;

            if (window.get_key(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                window.close();
            }

            window.poll_events();

            renderer.acquire_frame();

            renderer.start(); {
                renderer.draw(registry);
            } renderer.end();

            renderer.submit();
        }
    }

    Application::~Application() {
        char* str = new char[1000000]{};
        vmaBuildStatsString(context.allocator, &str, true);

        std::ofstream("../dump/stats.json") << str;

        vmaFreeStatsString(context.allocator, str);
    }
}