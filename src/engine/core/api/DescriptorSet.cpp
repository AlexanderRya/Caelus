#include <engine/core/api/MappedBuffer.hpp>
#include <engine/core/api/DescriptorSet.hpp>
#include <engine/core/api/VulkanContext.hpp>
#include <engine/Constants.hpp>
#include <engine/Logger.hpp>

namespace caelus::core::api {
    void DescriptorSet::create(const DescriptorSet::CreateInfo& info) {
        ctx = info.ctx;

        descriptor_sets.resize(meta::frames_in_flight);
        for (usize i = 0; i < meta::frames_in_flight; ++i) {
            descriptor_sets[i].create(info);
        }
    }

    void DescriptorSet::write(const std::vector<WriteBufferInfo>& write_info) {
        std::vector<DescriptorSet::SingleWriteBufferInfo> writes{};
        writes.reserve(write_info.size());
        for (usize i = 0; i < meta::frames_in_flight; ++i) {
            for (const auto& info : write_info) {
                auto& write = writes.emplace_back(); {
                    write.buffer_info = info.buffer_info[i];
                    write.binding = info.binding;
                    write.type = info.type;
                }
            }

            descriptor_sets[i].write(writes);

            writes.clear();
        }
    }

    void DescriptorSet::write(const DescriptorSet::WriteImageInfo& info) {
        for (auto& each : descriptor_sets) {
            each.write(info);
        }
    }

    SingleDescriptorSet& DescriptorSet::operator [](const usize idx) {
        return descriptor_sets[idx];
    }

    void SingleDescriptorSet::create(const DescriptorSet::CreateInfo& info) {
        ctx = info.ctx;

        u32 upper_bound = 128;

        vk::DescriptorSetVariableDescriptorCountAllocateInfo set_counts{}; {
            set_counts.descriptorSetCount = 1;
            set_counts.pDescriptorCounts = &upper_bound;
        }

        vk::DescriptorSetAllocateInfo allocate_info{}; {
            allocate_info.pNext = &set_counts;
            allocate_info.descriptorSetCount = 1;
            allocate_info.pSetLayouts = &info.layout;
            allocate_info.descriptorPool = ctx->descriptor_pool;
        }

        descriptor_set = ctx->device.logical.allocateDescriptorSets(allocate_info, ctx->dispatcher).back();
    }

    void SingleDescriptorSet::write(const std::vector<DescriptorSet::SingleWriteBufferInfo>& info) {
        std::vector<vk::WriteDescriptorSet> writes{}; {
            for (const auto& each : info) {
                auto& write = writes.emplace_back(); {
                    write.descriptorType = each.type;
                    write.dstBinding = each.binding;
                    write.dstArrayElement = 0;
                    write.dstSet = descriptor_set;
                    write.pTexelBufferView = nullptr;
                    write.descriptorCount = 1;
                    write.pBufferInfo = &each.buffer_info;
                }
            }
        }

        ctx->device.logical.updateDescriptorSets(writes, nullptr, ctx->dispatcher);
    }

    void SingleDescriptorSet::write(const DescriptorSet::SingleWriteBufferInfo& info) {
        vk::WriteDescriptorSet write{}; {
            write.descriptorType = info.type;
            write.dstBinding = info.binding;
            write.dstArrayElement = 0;
            write.dstSet = descriptor_set;
            write.pTexelBufferView = nullptr;
            write.descriptorCount = 1;
            write.pBufferInfo = &info.buffer_info;
        }

        ctx->device.logical.updateDescriptorSets(write, nullptr, ctx->dispatcher);
    }

    void SingleDescriptorSet::write(const DescriptorSet::WriteImageInfo& info) {
        vk::WriteDescriptorSet write{}; {
            write.descriptorType = info.type;
            write.dstBinding = info.binding;
            write.dstArrayElement = 0;
            write.dstSet = descriptor_set;
            write.pTexelBufferView = nullptr;
            write.descriptorCount = info.image_info.size();
            write.pImageInfo = info.image_info.data();
        }

        ctx->device.logical.updateDescriptorSets(write, nullptr, ctx->dispatcher);
    }

    vk::DescriptorSet SingleDescriptorSet::handle() const {
        return descriptor_set;
    }
} // namespace caelus::core::api