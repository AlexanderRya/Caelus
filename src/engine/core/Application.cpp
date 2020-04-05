#include <engine/core/api/renderer/ImGuiContext.hpp>
#include <engine/core/components/PointLight.hpp>
#include <engine/core/components/Transform.hpp>
#include <engine/core/components/Material.hpp>
#include <engine/core/components/Texture.hpp>
#include <engine/core/components/Camera.hpp>
#include <engine/core/components/Mesh.hpp>
#include <engine/core/api/Sampler.hpp>
#include <engine/core/Application.hpp>
#include <engine/core/Globals.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <entt/entt.hpp>

#include <GLFW/glfw3.h>

#include <fstream>

namespace caelus::core {
    Application::Application()
    : window(1280, 720, "Caelus"),
      context(api::make_vulkan_context(&window)),
      renderer(context) {
        api::make_imgui_context(context, window);
    }

    void Application::load() {
        scene.layouts[meta::PipelineLayoutType::eMeshGeneric] = api::make_generic_pipeline_layout(context);

        scene.samplers[meta::SamplerType::eDefault] = api::make_default_sampler(context);

        api::Pipeline::CreateInfo create_info{}; {
            create_info.ctx = &context;
            create_info.vertex_path = "../resources/shaders/generic.vert.spv";
            create_info.fragment_path = "../resources/shaders/generic.frag.spv";
            create_info.layout = scene.layouts[meta::PipelineLayoutType::eMeshGeneric];
        }

        scene.pipelines[meta::PipelineType::eMeshGeneric] = api::make_generic_pipeline(create_info);

        scene.textures.emplace_back(context, scene.samplers[meta::SamplerType::eDefault]).load("../resources/textures/dirt.jpg");
    }

    void Application::run() {
        auto orange_cube = scene.entities.emplace_back(scene.registry.create()); {
            scene.registry.emplace<components::Mesh>(orange_cube, components::Mesh{
                .vertex_buffer_idx = 2,
                .vertex_count = 36,
            });

            scene.registry.emplace<components::Transform>(orange_cube, components::Transform{
                .position = { 0.0f, 0.0f, 0.0f },
                .scale = { 0.5f, 0.5f, 0.5f },
                .rotation = 0
            });

            scene.registry.emplace<components::Material>(orange_cube, components::Material{
                .instance = {
                    .color = { 1.0f, 0.5f, 0.31f, 1.0f },
                    .texture_idx = -1
                },

                .pipeline = scene.pipelines[meta::PipelineType::eMeshGeneric].handle
            });
        }

        auto light = scene.entities.emplace_back(scene.registry.create()) = scene.registry.create(); {
            scene.registry.emplace<components::PointLight>(light);

            scene.registry.emplace<components::Transform>(light, components::Transform{
                .position = { 4.2f, 3.0f, 5.0f },
                .scale = { 0.2f, 0.2f, 0.2f },
                .rotation = 0
            });
        }

        auto white_cube = scene.entities.emplace_back(scene.registry.create()); {
            scene.registry.emplace<components::Mesh>(white_cube, components::Mesh{
                .vertex_buffer_idx = 2,
                .vertex_count = 36,
            });

            scene.registry.emplace<components::Transform>(white_cube, components::Transform{
                .position = { 4.2f, 3.0f, 5.0f },
                .scale = { 0.2f, 0.2f, 0.2f },
                .rotation = 0
            });

            scene.registry.emplace<components::Material>(white_cube, components::Material{
                .instance = {
                    .color = { 1.0f, 1.0f, 1.0f, 1.0f },
                    .texture_idx = -1
                },

                .pipeline = scene.pipelines[meta::PipelineType::eMeshGeneric].handle
            });
        }

        renderer.init(scene);

        while (!window.should_close()) {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            const f32 frame_time = glfwGetTime();
            delta_time = frame_time - last_frame;
            last_frame = frame_time;

            if (window.get_key(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                window.close();
            }

            if (Window::captured) {
                camera.move(window);
            }

            window.poll_events();

            // Render
            renderer.build(scene);

            renderer.acquire_frame();

            renderer.start(); {
                ImGui::Render();
                renderer.draw(scene);
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