#include <engine/core/components/Transform.hpp>
#include <engine/core/components/Material.hpp>
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

    void Application::load() {
        graph.layouts[meta::PipelineLayoutType::eMeshGeneric] = api::make_generic_pipeline_layout(context);

        graph.samplers[meta::SamplerType::eDefault] = api::make_default_sampler(context);

        api::Pipeline::CreateInfo create_info{}; {
            create_info.ctx = &context;
            create_info.vertex_path = "../resources/shaders/generic.vert.spv";
            create_info.fragment_path = "../resources/shaders/generic.frag.spv";
            create_info.layout = graph.layouts[meta::PipelineLayoutType::eMeshGeneric];
        }

        graph.pipelines[meta::PipelineType::eMeshGeneric] = api::make_generic_pipeline(create_info);

        graph.textures.emplace_back(context, graph.samplers[meta::SamplerType::eDefault]).load("../resources/textures/dirt.jpg");
    }

    void Application::run() {
        auto& quad = graph.objects.emplace_back(graph.registry.create()); {
            graph.registry.emplace<components::Mesh>(quad, components::Mesh{
                .vertex_buffer_idx = 2,
                .vertex_count = 36,
            });

            graph.registry.emplace<components::Transform>(quad, components::Transform{
                .instances = {
                    components::Transform::Instance{
                        .position = { 0.0f, 0.0f, 0.0f },
                        .scale = { 0.5f, 0.5f, 0.5f },
                        .rotation = 0
                    },
                    components::Transform::Instance{
                        .position = { 2.2f, 2.0f, 3.0f },
                        .scale = { 0.2f, 0.2f, 0.2f },
                        .rotation = 0
                    }
                }
            });

            graph.registry.emplace<components::Material>(quad, components::Material{
                .materials = {
                    components::Material::Instance{
                        .color = { 1.0f, 1.0f, 1.0f, 1.0f },
                        .texture_idx = 0
                    },
                    components::Material::Instance{
                        .color = { 1.0f, 1.0f, 1.0f, 1.0f },
                        .texture_idx = -1
                    },
                }
            });
        }

        renderer.build(graph);

        while (!window.should_close()) {
            const f32 frame_time = glfwGetTime();
            delta_time = frame_time - last_frame;
            last_frame = frame_time;

            if (window.get_key(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                window.close();
            }

            camera.move(window);

            window.poll_events();

            // Render
            renderer.acquire_frame();

            renderer.start(); {
                renderer.draw(graph);
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