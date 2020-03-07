#ifndef CAELUS_PIPELINE_HPP
#define CAELUS_PIPELINE_HPP

#include <engine/Forwards.hpp>

#include <filesystem>

#include <vulkan/vulkan.hpp>

namespace caelus::core::api {
    struct PipelineLayout {
        vk::PipelineLayout pipeline{};
        vk::DescriptorSetLayout set{};
    };

    struct Pipeline {
        struct CreateInfo {
            const VulkanContext* ctx;

            std::filesystem::path vertex_path{};
            std::filesystem::path fragment_path{};

            PipelineLayout layout{};
        };

        PipelineLayout layout{};

        vk::Pipeline handle{};
    };

    [[nodiscard]] PipelineLayout make_generic_pipeline_layout(const VulkanContext&);
    [[nodiscard]] Pipeline make_generic_pipeline(const Pipeline::CreateInfo&);
} // namespace caelus::core::api

#endif //CAELUS_PIPELINE_HPP
