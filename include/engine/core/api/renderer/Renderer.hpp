#ifndef CAELUS_RENDERER_HPP
#define CAELUS_RENDERER_HPP

#include <engine/core/api/VertexBuffer.hpp>
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
        std::unordered_map<u64, api::VertexBuffer> vertex_buffers;

        u32 frames_rendered{};
        u32 image_index{};
        u32 current_frame{};
    public:
        explicit Renderer(const VulkanContext&);

        // Loading
        void init_rendering_data();

        // Drawing
        void acquire_frame();
        void build();
        void draw();

    };
} // namespace caelus::core::api

#endif //CAELUS_RENDERER_HPP
