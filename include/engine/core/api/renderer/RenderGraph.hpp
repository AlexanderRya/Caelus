#ifndef CAELUS_RENDERGRAPH_HPP
#define CAELUS_RENDERGRAPH_HPP

#include <engine/core/api/Pipeline.hpp>
#include <engine/Constants.hpp>

#include <entt/entt.hpp>

#include <vector>

namespace caelus::core::api {
    struct RenderGraph {
        std::unordered_map<meta::PipelineType, api::Pipeline> pipelines;
        std::unordered_map<meta::PipelineLayoutType, api::PipelineLayout> layouts;
        std::unordered_map<meta::SamplerType, vk::Sampler> samplers;

        std::vector<entt::entity> objects;
        entt::registry registry;
    };
} // namespace caelus::core::api

#endif //CAELUS_RENDERGRAPH_HPP
