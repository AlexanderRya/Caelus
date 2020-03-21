#include <engine/core/api/renderer/Renderer.hpp>
#include <engine/core/components/Transform.hpp>
#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/DescriptorSet.hpp>
#include <engine/core/api/CommandBuffer.hpp>
#include <engine/core/api/MappedBuffer.hpp>
#include <engine/core/api/VertexBuffer.hpp>
#include <engine/core/components/Mesh.hpp>
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

        // Primitive geometry
        vertex_buffers.emplace_back(api::make_vertex_buffer(components::generate_triangle_geometry(), ctx));
        vertex_buffers.emplace_back(api::make_vertex_buffer(components::generate_quad_geometry(), ctx));
    }

    void Renderer::build(RenderGraph& graph) {
        auto mesh_view = graph.registry.view<components::Mesh, components::Transform>();

        for (auto& entity : mesh_view) {
            auto& mesh = mesh_view.get<components::Mesh>(entity);

            api::DescriptorSet::CreateInfo descriptor_set_info{}; {
                descriptor_set_info.ctx = &ctx;
                descriptor_set_info.layout = graph.layouts[meta::PipelineLayoutType::MeshGeneric].set;
            }

            mesh.descriptor_set.create(descriptor_set_info);

            api::MappedBuffer::CreateInfo buffer_info{}; {
                buffer_info.ctx = &ctx;
                buffer_info.buffer_usage = vk::BufferUsageFlagBits::eStorageBuffer;
                buffer_info.type_size = sizeof(components::detail::InstanceGLSL);
            }

            mesh.instance_buffer.create(buffer_info);

            std::vector<api::DescriptorSet::WriteInfo> write_info(1); {
                write_info[0].buffer_info = mesh.instance_buffer.get_info();
                write_info[0].binding = static_cast<u32>(meta::PipelineBinding::Instance);
                write_info[0].type = vk::DescriptorType::eStorageBuffer;
            }

            mesh.descriptor_set.write(write_info);
        }
    }

    void Renderer::update(components::Mesh& mesh, components::Transform& transform) {
        std::vector<components::detail::InstanceGLSL> instances;
        instances.reserve(transform.instances.size());

        for (const auto& instance : transform.instances) {
            auto& model = instances.emplace_back().model;

            model = glm::mat4(1.0f);
            model = glm::translate(model, instance.position);
            model = glm::scale(model, instance.scale);
            model = glm::rotate(model, instance.rotation, { 0.0f, 0.0f, 1.0f });
        }

        auto& current_buffer = mesh.instance_buffer[current_frame];

        if (current_buffer.size() != instances.size()) {
            current_buffer.write(instances.data(), instances.size());

            api::DescriptorSet::WriteInfo write_info{}; {
                write_info.binding = static_cast<u32>(meta::PipelineBinding::Instance);
                write_info.type = vk::DescriptorType::eStorageBuffer;
                write_info.buffer_info = { current_buffer.get_info() };
            }

            mesh.descriptor_set[current_frame].write(write_info);
        } else {
            current_buffer.write(instances.data(), instances.size());
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
            clear_values[1].depthStencil = vk::ClearDepthStencilValue{ { 1.0f, 0 } };
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

    void Renderer::draw(RenderGraph& graph) {
        auto& command_buffer = command_buffers[image_index];
        auto mesh_view = graph.registry.view<components::Mesh, components::Transform>();

        command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graph.pipelines[meta::PipelineType::MeshGeneric].handle, ctx.dispatcher);
        for (const auto& each : mesh_view) {
            auto [mesh, transform] = mesh_view.get<components::Mesh, components::Transform>(each);

            update(mesh, transform);

            command_buffer.bindVertexBuffers(0, vertex_buffers[mesh.vertex_buffer_idx].handle, static_cast<vk::DeviceSize>(0), ctx.dispatcher);
            command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graph.layouts[meta::PipelineLayoutType::MeshGeneric].pipeline, 0, mesh.descriptor_set[current_frame].handle(), nullptr, ctx.dispatcher);
            command_buffer.draw(mesh.vertex_count, mesh.instance_buffer[current_frame].size(), 0, 0, ctx.dispatcher);
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
} // namespace caelus::core::api
