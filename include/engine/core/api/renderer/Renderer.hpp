#ifndef CAELUS_RENDERER_HPP
#define CAELUS_RENDERER_HPP

#include <engine/core/api/renderer/Scene.hpp>
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
        u32 image_index{};
        u32 current_frame{};

        void update_camera(Scene&);
        void update_transforms(components::Mesh&, components::Transform&);
        void update_materials(components::Mesh&, components::Material&);
    public:
        static inline u32 frames_rendered{};

        explicit Renderer(const VulkanContext&);

        void allocate(const Scene&, components::Mesh&);
        void build(Scene&);

        // Drawing
        u32 acquire_frame();
        void start();
        void draw(Scene&);
        void end();
        void submit();
    };
} // namespace caelus::core::api

#endif //CAELUS_RENDERER_HPP
