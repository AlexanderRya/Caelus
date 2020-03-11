#include <engine/core/components/MeshRenderer.hpp>
#include <engine/core/api/renderer/Renderer.hpp>
#include <engine/core/components/CameraData.hpp>
#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/DescriptorSet.hpp>
#include <engine/core/api/CommandBuffer.hpp>
#include <engine/core/api/MappedBuffer.hpp>
#include <engine/core/api/VertexBuffer.hpp>
#include <engine/core/components/Mesh.hpp>
#include <engine/core/api/Pipeline.hpp>
#include <engine/core/api/Sampler.hpp>
#include <engine/core/Globals.hpp>
#include <engine/Constants.hpp>
#include <engine/Logger.hpp>

namespace caelus::core::api {
    Renderer::Renderer(const api::VulkanContext& context)
    : ctx(context) {
        command_buffers = api::make_rendering_command_buffers(ctx);

        vk::SemaphoreCreateInfo semaphore_create_info{};

        image_available.reserve(meta::frames_in_flight);
        render_finished.reserve(meta::frames_in_flight);

        for (u64 i = 0; i < meta::frames_in_flight; ++i) {
            image_available.emplace_back(ctx.device.logical.createSemaphore(semaphore_create_info, nullptr, ctx.dispatcher));
            render_finished.emplace_back(ctx.device.logical.createSemaphore(semaphore_create_info, nullptr, ctx.dispatcher));
        }

        frames_in_flight.resize(meta::frames_in_flight, nullptr);

        vertex_buffers.emplace_back(api::make_vertex_buffer(components::generate_triangle_geometry(), ctx));
        vertex_buffers.emplace_back(api::make_vertex_buffer(components::generate_quad_geometry(), ctx));

        layouts[meta::PipelineLayoutType::MeshGeneric] = api::make_generic_pipeline_layout(context);

        samplers[meta::SamplerType::Default] = api::make_default_sampler(context);

        api::Pipeline::CreateInfo create_info{}; {
            create_info.ctx = &context;
            create_info.vertex_path = "../resources/shaders/generic.vert.spv";
            create_info.fragment_path = "../resources/shaders/generic.frag.spv";
            create_info.layout = layouts[meta::PipelineLayoutType::MeshGeneric];
        }

        pipelines[meta::PipelineType::MeshGeneric] = api::make_generic_pipeline(create_info);
    }

    void Renderer::build(entt::registry& registry) {
        camera_entity = registry.create();

        /* Camera buffer */ {
            api::MappedBuffer::CreateInfo buffer_info{}; {
                buffer_info.ctx = &ctx;
                buffer_info.type_size = sizeof(glm::mat4);
                buffer_info.buffer_usage = vk::BufferUsageFlagBits::eUniformBuffer;
            }

            components::CameraData camera_data{}; {
                camera_data.buffer.create(buffer_info);
            }

            registry.assign<components::CameraData>(camera_entity, std::move(camera_data));
        }

        auto mesh_view = registry.view<components::Mesh, components::MeshRenderer>();
        auto camera_buffer_info = registry.get<components::CameraData>(camera_entity).buffer.get_info();

        for (auto& entity : mesh_view) {
            auto& mesh_renderer = mesh_view.get<components::MeshRenderer>(entity);

            api::DescriptorSet::CreateInfo create_info{}; {
                create_info.ctx = &ctx;
                create_info.layout = layouts[meta::PipelineLayoutType::MeshGeneric].set;
            }

            mesh_renderer.descriptor_set.create(create_info);

            api::DescriptorSet::WriteInfo write_info{}; {
                write_info.buffer_info = camera_buffer_info;
                write_info.type = vk::DescriptorType::eUniformBuffer;
                write_info.binding = static_cast<u32>(meta::PipelineBinding::Camera);
            }

            mesh_renderer.descriptor_set.write(write_info);
        }
    }

    u32 Renderer::acquire_frame() {
        image_index = ctx.device.logical.acquireNextImageKHR(ctx.swapchain.handle, -1, image_available[current_frame], nullptr, ctx.dispatcher).value;

        if (!frames_in_flight[current_frame]) {
            vk::FenceCreateInfo fence_create_info{}; {
                fence_create_info.flags = vk::FenceCreateFlagBits::eSignaled;
            }

            frames_in_flight[current_frame] = ctx.device.logical.createFence(fence_create_info, nullptr, ctx.dispatcher);
        }

        ctx.device.logical.waitForFences(frames_in_flight[current_frame], true, -1, ctx.dispatcher);

        return current_frame;
    }

    void Renderer::start() {
        auto& command_buffer = command_buffers[image_index];

        vk::CommandBufferBeginInfo begin_info{}; {
            begin_info.flags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        }

        command_buffer.begin(begin_info, ctx.dispatcher);

        std::array<vk::ClearValue, 2> clear_values{}; {
            clear_values[0].color = vk::ClearColorValue{ std::array { 0.02f, 0.02f, 0.02f, 0.0f } };
            clear_values[1].depthStencil = { { 1.0f, 0 } };
        }

        vk::RenderPassBeginInfo render_pass_begin_info{}; {
            render_pass_begin_info.renderArea.extent = ctx.swapchain.extent;
            render_pass_begin_info.framebuffer = ctx.default_framebuffers[image_index];
            render_pass_begin_info.renderPass = ctx.default_render_pass;
            render_pass_begin_info.clearValueCount = clear_values.size();
            render_pass_begin_info.pClearValues = clear_values.data();
        }

        vk::Viewport viewport{}; {
            viewport.width = ctx.swapchain.extent.width;
            viewport.height = ctx.swapchain.extent.height;
            viewport.x = 0;
            viewport.y = 0;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
        }

        vk::Rect2D scissor{}; {
            scissor.extent = ctx.swapchain.extent;
            scissor.offset = { { 0, 0 } };
        }

        command_buffer.setViewport(0, viewport, ctx.dispatcher);
        command_buffer.setScissor(0, scissor, ctx.dispatcher);

        command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline, ctx.dispatcher);
    }

    void Renderer::draw(entt::registry& registry) {
        auto& command_buffer = command_buffers[image_index];
        auto view = registry.view<components::Mesh, components::MeshRenderer>();

        update_camera(registry);

        command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines[meta::PipelineType::MeshGeneric].handle, ctx.dispatcher);

        for (const auto& entity : view) {
            auto [mesh, mesh_renderer] = view.get<components::Mesh, components::MeshRenderer>(entity);

            command_buffer.bindVertexBuffers(0, vertex_buffers[mesh.vertex_buffer_idx].handle, static_cast<vk::DeviceSize>(0), ctx.dispatcher);
            command_buffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                layouts[meta::PipelineLayoutType::MeshGeneric].pipeline,
                0,
                mesh_renderer.descriptor_set[current_frame],
                nullptr,
                ctx.dispatcher);
            command_buffer.draw(mesh.vertex_count, 1, 0, 0, ctx.dispatcher);
        }
    }

    void Renderer::end() {
        auto& command_buffer = command_buffers[image_index];

        command_buffer.endRenderPass(ctx.dispatcher);

        command_buffer.end(ctx.dispatcher);
    }

    void Renderer::submit() {
        vk::PipelineStageFlags wait_mask{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
        vk::SubmitInfo submit_info{}; {
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &command_buffers[image_index];
            submit_info.pWaitDstStageMask = &wait_mask;
            submit_info.waitSemaphoreCount = 1;
            submit_info.pWaitSemaphores = &image_available[current_frame];
            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = &render_finished[current_frame];
        }

        ctx.device.logical.resetFences(frames_in_flight[current_frame], ctx.dispatcher);
        ctx.device.queue.submit(submit_info, frames_in_flight[current_frame], ctx.dispatcher);

        vk::PresentInfoKHR present_info{}; {
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = &render_finished[current_frame];
            present_info.swapchainCount = 1;
            present_info.pSwapchains = &ctx.swapchain.handle;
            present_info.pImageIndices = &image_index;
        }

        ctx.device.queue.presentKHR(present_info, ctx.dispatcher);

        ++frames_rendered;
        current_frame = (current_frame + 1) % meta::frames_in_flight;
    }

    void Renderer::update_camera(entt::registry& registry) {
        auto& data = registry.get<components::CameraData>(camera_entity);

        glm::mat4 pv_matrix = glm::mat4(1.0f);

        data.buffer[current_frame].write(&pv_matrix, 1);
    }
} // namespace caelus::core::api
