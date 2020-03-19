#ifndef CAELUS_DESCRIPTORSET_HPP
#define CAELUS_DESCRIPTORSET_HPP

#include <engine/Forwards.hpp>
#include <engine/Types.hpp>

#include <vulkan/vulkan.hpp>

namespace caelus::core::api {
    class SingleDescriptorSet;

    class DescriptorSet {
        std::vector<SingleDescriptorSet> descriptor_sets{};
        const api::VulkanContext* ctx{};
    public:
        struct WriteInfo {
            u64 binding{};
            vk::DescriptorType type{};
            std::vector<vk::DescriptorBufferInfo> buffer_info{};
        };

        struct CreateInfo {
            const api::VulkanContext* ctx{};
            vk::DescriptorSetLayout layout{};
        };

        DescriptorSet() = default;

        void create(const CreateInfo&);

        void write(const std::vector<WriteInfo>&);
        void write(const WriteInfo&);

        [[nodiscard]] SingleDescriptorSet operator [](const usize) const;
    };

    class SingleDescriptorSet {
        vk::DescriptorSet descriptor_set{};
        const api::VulkanContext* ctx{};
    public:
        void create(const DescriptorSet::CreateInfo&);

        void write(const std::vector<DescriptorSet::WriteInfo>&);
        void write(const DescriptorSet::WriteInfo&);

        [[nodiscard]] vk::DescriptorSet handle() const;
    };
} // namespace caelus::core::api

#endif //CAELUS_DESCRIPTORSET_HPP
