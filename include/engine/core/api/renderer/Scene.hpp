#ifndef CAELUS_SCENE_HPP
#define CAELUS_SCENE_HPP

#include <engine/core/api/MappedBuffer.hpp>
#include <engine/core/api/Pipeline.hpp>
#include <engine/Constants.hpp>
#include <engine/Forwards.hpp>

#include <entt/entt.hpp>

#include <vector>

namespace caelus::core::api {
    struct Scene {
        std::unordered_map<meta::PipelineType, api::Pipeline> pipelines;
        std::unordered_map<meta::PipelineLayoutType, api::PipelineLayout> layouts;
        std::unordered_map<meta::SamplerType, vk::Sampler> samplers;
        std::vector<components::Texture> textures;

        std::vector<entt::entity> entities{};
        entt::registry registry{};
    };
} // namespace caelus::core::api

#endif //CAELUS_SCENE_HPP
