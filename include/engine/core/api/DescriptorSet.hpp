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
        struct SingleWriteBufferInfo {
            u64 binding{};
            vk::DescriptorType type{};
            vk::DescriptorBufferInfo buffer_info{};
        };

        struct WriteBufferInfo {
            u64 binding{};
            vk::DescriptorType type{};
            std::vector<vk::DescriptorBufferInfo> buffer_info{};
        };

        struct WriteImageInfo {
            u64 binding{};
            vk::DescriptorType type{};
            std::vector<vk::DescriptorImageInfo> image_info{};
        };

        struct CreateInfo {
            const api::VulkanContext* ctx{};
            vk::DescriptorSetLayout layout{};
        };

        DescriptorSet() = default;

        void create(const CreateInfo&);

        void write(const std::vector<WriteBufferInfo>&);
        void write(const WriteImageInfo&);

        [[nodiscard]] SingleDescriptorSet& operator [](const usize);
    };

    class SingleDescriptorSet {
        vk::DescriptorSet descriptor_set{};
        const api::VulkanContext* ctx{};
    public:
        void create(const DescriptorSet::CreateInfo&);

        void write(const std::vector<DescriptorSet::SingleWriteBufferInfo>&);
        void write(const DescriptorSet::SingleWriteBufferInfo&);
        void write(const DescriptorSet::WriteImageInfo&);

        [[nodiscard]] vk::DescriptorSet handle() const;
    };
} // namespace caelus::core::api

#endif //CAELUS_DESCRIPTORSET_HPP
