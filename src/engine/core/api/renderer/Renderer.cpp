#include <engine/core/api/renderer/Renderer.hpp>
#include <engine/core/components/Transform.hpp>
#include <engine/core/components/Material.hpp>
#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/DescriptorSet.hpp>
#include <engine/core/api/CommandBuffer.hpp>
#include <engine/core/components/Camera.hpp>
#include <engine/core/api/MappedBuffer.hpp>
#include <engine/core/api/VertexBuffer.hpp>
#include <engine/core/components/Mesh.hpp>
#include <engine/core/api/Pipeline.hpp>
#include <engine/core/api/Sampler.hpp>
#include <engine/core/Globals.hpp>
#include <engine/Constants.hpp>
#include <engine/Logger.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
        vertex_buffers.emplace_back(api::make_vertex_buffer(components::generate_cube_geometry(), ctx));
    }

    void Renderer::init(Scene& scene) {
        api::MappedBuffer::CreateInfo info{}; {
            info.ctx = &ctx;
            info.buffer_usage = vk::BufferUsageFlagBits::eUniformBuffer;
            info.type_size = sizeof(glm::mat4);
        }
        camera_buffer.create(info);

        api::DescriptorSet::CreateInfo descriptor_set_info{}; {
            descriptor_set_info.ctx = &ctx;
            descriptor_set_info.layout = scene.layouts.at(meta::PipelineLayoutType::eMeshGeneric).set;
        }

        descriptor_set.create(descriptor_set_info);

        api::MappedBuffer::CreateInfo instance_buffer_info{}; {
            instance_buffer_info.ctx = &ctx;
            instance_buffer_info.buffer_usage = vk::BufferUsageFlagBits::eStorageBuffer;
            instance_buffer_info.type_size = sizeof(glm::mat4);
        }

        instance_buffer.create(instance_buffer_info);

        api::MappedBuffer::CreateInfo material_buffer_info{}; {
            material_buffer_info.ctx = &ctx;
            material_buffer_info.buffer_usage = vk::BufferUsageFlagBits::eStorageBuffer;
            material_buffer_info.type_size = sizeof(components::Material::Instance);
        }

        material_buffer.create(material_buffer_info);

        api::DescriptorSet::WriteImageInfo write_image_info{}; {
            write_image_info.image_info = {
                scene.textures[0].get_info()
            };
            write_image_info.binding = static_cast<u32>(meta::PipelineBinding::eTexture);
            write_image_info.type = vk::DescriptorType::eCombinedImageSampler;
        }

        std::vector<api::DescriptorSet::WriteBufferInfo> write_buffer_info(3); {
            write_buffer_info[0].buffer_info = camera_buffer.get_info();
            write_buffer_info[0].binding = static_cast<u32>(meta::PipelineBinding::eCamera);
            write_buffer_info[0].type = vk::DescriptorType::eUniformBuffer;

            write_buffer_info[1].buffer_info = instance_buffer.get_info();
            write_buffer_info[1].binding = static_cast<u32>(meta::PipelineBinding::eInstance);
            write_buffer_info[1].type = vk::DescriptorType::eStorageBuffer;

            write_buffer_info[2].buffer_info = material_buffer.get_info();
            write_buffer_info[2].binding = static_cast<u32>(meta::PipelineBinding::eMaterial);
            write_buffer_info[2].type = vk::DescriptorType::eStorageBuffer;
        }

        descriptor_set.write(write_buffer_info);
        descriptor_set.write(write_image_info);
    }

    void Renderer::build(Scene& scene) {
        auto mesh_view = scene.registry.view<components::Mesh, components::Transform, components::Material>();
        materials.clear();
        transforms.clear();
        draw_commands.clear();

        materials.reserve(mesh_view.size());
        transforms.reserve(mesh_view.size());
        draw_commands.reserve(mesh_view.size());

        for (auto& entity : mesh_view) {
            auto [mesh, transform, material] = mesh_view.get<components::Mesh, components::Transform, components::Material>(entity);

            DrawCommand command{
                .mesh = &mesh,
                .material_idx = static_cast<i32>(materials.size()),
                .transform_idx = static_cast<i32>(transforms.size())
            };

            transforms.emplace_back(transform.model());
            materials.emplace_back(material.instance);
            draw_commands.emplace_back(command);
        }
    }

    void Renderer::update_camera() {
        auto projection = glm::perspective(
            glm::radians(70.f),
            ctx.swapchain.extent.width / static_cast<float>(ctx.swapchain.extent.height),
            0.05f,
            1000.f);

        projection[1][1] *= -1;

        auto proj_view = projection * camera.get_view_mat();

        camera_buffer[current_frame].write(&proj_view, 1);
    }

    void Renderer::update_transforms() {
        auto& current_buffer = instance_buffer[current_frame];

        if (current_buffer.size() != transforms.size()) {
            current_buffer.write(transforms.data(), transforms.size());

            api::DescriptorSet::SingleWriteBufferInfo write_info{}; {
                write_info.binding = static_cast<u32>(meta::PipelineBinding::eInstance);
                write_info.type = vk::DescriptorType::eStorageBuffer;
                write_info.buffer_info = current_buffer.get_info();
            }

            descriptor_set[current_frame].write(write_info);
        } else {
            current_buffer.write(transforms.data(), transforms.size());
        }
    }

    void Renderer::update_materials() {
        auto& current_buffer = material_buffer[current_frame];

        if (current_buffer.size() != materials.size()) {
            current_buffer.write(materials.data(), materials.size());

            api::DescriptorSet::SingleWriteBufferInfo write_info{}; {
                write_info.binding = static_cast<u32>(meta::PipelineBinding::eMaterial);
                write_info.type = vk::DescriptorType::eStorageBuffer;
                write_info.buffer_info = current_buffer.get_info();
            }

            descriptor_set[current_frame].write(write_info);
        } else {
            current_buffer.write(materials.data(), materials.size());
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
            clear_values[0].color = vk::ClearColorValue{ std::array{ 0.01f, 0.01f, 0.01f, 0.0f } };
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

    void Renderer::draw(Scene& scene) {
        auto& command_buffer = command_buffers[image_index];

        update_camera();
        update_transforms();
        update_materials();

        command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, scene.pipelines[meta::PipelineType::eMeshGeneric].handle, ctx.dispatcher);
        command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, scene.layouts[meta::PipelineLayoutType::eMeshGeneric].pipeline, 0, descriptor_set[current_frame].handle(), nullptr, ctx.dispatcher);

        for (auto& command : draw_commands) {
            auto& mesh = *command.mesh;

            i32 indices[] {
                command.transform_idx,
                command.material_idx
            };

            command_buffer.bindVertexBuffers(0, vertex_buffers[mesh.vertex_buffer_idx].handle, static_cast<vk::DeviceSize>(0), ctx.dispatcher);
            command_buffer.pushConstants(scene.layouts[meta::PipelineLayoutType::eMeshGeneric].pipeline, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0u, 2 * sizeof(i32), indices, ctx.dispatcher);
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
} // namespace caelus::core::api
