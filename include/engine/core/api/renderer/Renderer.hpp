#ifndef CAELUS_RENDERER_HPP
#define CAELUS_RENDERER_HPP

#include <engine/core/api/renderer/RenderGraph.hpp>
#include <engine/core/components/Transform.hpp>
#include <engine/core/components/Texture.hpp>
#include <engine/core/api/DescriptorSet.hpp>
#include <engine/core/api/MappedBuffer.hpp>
#include <engine/core/api/VertexBuffer.hpp>
#include <engine/core/components/Mesh.hpp>
#include <engine/core/api/Pipeline.hpp>
#include <engine/core/api/Buffer.hpp>
#include <engine/Constants.hpp>
#include <engine/Forwards.hpp>
#include <engine/Types.hpp>

#include <unordered_map>
#include <vector>

namespace caelus::core::api {
    class Renderer {
        std::vector<vk::Semaphore> image_available;
        std::vector<vk::Semaphore> render_finished;
        std::vector<vk::Fence> frames_in_flight;

        std::vector<vk::CommandBuffer> command_buffers;

        const VulkanContext& ctx;

        // Drawing stuff
        std::vector<api::Buffer> vertex_buffers;

        u32 frames_rendered{};
        u32 image_index{};
        u32 current_frame{};

    public:
        explicit Renderer(const VulkanContext&);

        void build(RenderGraph&);
        void update(components::Mesh&, components::Transform&);

        // Drawing
        u32 acquire_frame();
        void start();
        void draw(RenderGraph&);
        void end();
        void submit();
    };
} // namespace caelus::core::api

#endif //CAELUS_RENDERER_HPP
