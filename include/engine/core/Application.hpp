#ifndef CAELUS_CAELUS_HPP
#define CAELUS_CAELUS_HPP

#include <engine/core/api/renderer/Scene.hpp>
#include <engine/core/api/renderer/Renderer.hpp>
#include <engine/core/components/Texture.hpp>
#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/Pipeline.hpp>
#include <engine/core/Window.hpp>
#include <engine/Forwards.hpp>

#include <entt/entt.hpp>

#include <unordered_map>

namespace caelus::core {
    class Application {
        Window window;
        api::VulkanContext context;
        api::Renderer renderer;

        api::Scene scene;
    public:
        Application();
        ~Application();

        void load();
        void run();
    };
} // namespace caelus::core

#endif //CAELUS_CAELUS_HPP
