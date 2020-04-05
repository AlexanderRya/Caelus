#ifndef CAELUS_RENDERER_HPP
#define CAELUS_RENDERER_HPP

#include <engine/core/components/Transform.hpp>
#include <engine/core/components/Material.hpp>
#include <engine/core/api/renderer/Scene.hpp>
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
        struct DrawCommand {
            const components::Mesh* mesh{};
            i32 material_idx{};
            i32 transform_idx{};
        };

        std::vector<vk::Semaphore> image_available;
        std::vector<vk::Semaphore> render_finished;
        std::vector<vk::Fence> frames_in_flight;

        std::vector<vk::CommandBuffer> command_buffers;

        const VulkanContext& ctx;

        // Drawing stuff
        std::vector<api::Buffer> vertex_buffers;
        std::vector<glm::mat4> transforms;
        std::vector<components::Material::Instance> materials;
        std::vector<DrawCommand> draw_commands;

        api::DescriptorSet descriptor_set{};
        api::MappedBuffer instance_buffer{};
        api::MappedBuffer material_buffer{};
        api::MappedBuffer camera_buffer{};

        u32 image_index{};
        u32 current_frame{};

        void update_camera();
        void update_transforms();
        void update_materials();
    public:
        static inline u32 frames_rendered{};

        explicit Renderer(const VulkanContext&);

        void init(Scene&);
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
