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

    void DescriptorSet::write(const std::vector<WriteInfo>& write_info) {
        std::vector<vk::WriteDescriptorSet> writes;
        writes.resize(write_info.size());

        for (usize i = 0; i < meta::frames_in_flight; ++i) {
            for (usize j = 0; j < write_info.size(); ++j) {
                writes[j].descriptorType = write_info[j].type;
                writes[j].dstBinding = write_info[j].binding;
                writes[j].dstArrayElement = 0;
                writes[j].dstSet = descriptor_sets[i].handle();
                writes[j].pTexelBufferView = nullptr;
                if (!write_info[j].buffer_info.empty()) {
                    writes[j].descriptorCount = 1;
                    writes[j].pBufferInfo = &write_info[j].buffer_info[i];
                }
            }

            ctx->device.logical.updateDescriptorSets(writes, nullptr, ctx->dispatcher);
        }
    }

    void DescriptorSet::write(const WriteInfo& write_info) {
        for (usize i = 0; i < meta::frames_in_flight; ++i) {
            vk::WriteDescriptorSet write{}; {
                write.descriptorType = write_info.type;
                write.dstBinding = write_info.binding;
                write.dstArrayElement = 0;
                write.dstSet = descriptor_sets[i].handle();
                write.pTexelBufferView = nullptr;
                if (!write_info.buffer_info.empty()) {
                    write.descriptorCount = 1;
                    write.pBufferInfo = &write_info.buffer_info[i];
                }
            }

            ctx->device.logical.updateDescriptorSets(write, nullptr, ctx->dispatcher);
        }
    }

    SingleDescriptorSet DescriptorSet::operator [](const usize idx) const {
        return descriptor_sets[idx];
    }

    void SingleDescriptorSet::create(const DescriptorSet::CreateInfo& info) {
        ctx = info.ctx;

        vk::DescriptorSetAllocateInfo allocate_info{}; {
            allocate_info.descriptorSetCount = 1;
            allocate_info.pSetLayouts = &info.layout;
            allocate_info.descriptorPool = ctx->descriptor_pool;
        }

        descriptor_set = ctx->device.logical.allocateDescriptorSets(allocate_info, ctx->dispatcher).back();
    }

    void SingleDescriptorSet::write(const DescriptorSet::WriteInfo& write_info) {
        vk::WriteDescriptorSet write{}; {
            write.descriptorType = write_info.type;
            write.dstBinding = write_info.binding;
            write.dstArrayElement = 0;
            write.dstSet = descriptor_set;
            write.pTexelBufferView = nullptr;
            if (!write_info.buffer_info.empty()) {
                write.descriptorCount = 1;
                write.pBufferInfo = &write_info.buffer_info[0];
            }
        }

        ctx->device.logical.updateDescriptorSets(write, nullptr, ctx->dispatcher);
    }

    void SingleDescriptorSet::write(const std::vector<DescriptorSet::WriteInfo>& write_info) {
        std::vector<vk::WriteDescriptorSet> writes;
        writes.resize(write_info.size());

            for (usize j = 0; j < write_info.size(); ++j) {
                writes[j].descriptorType = write_info[j].type;
                writes[j].dstBinding = write_info[j].binding;
                writes[j].dstArrayElement = 0;
                writes[j].dstSet = descriptor_set;
                writes[j].pTexelBufferView = nullptr;
                if (!write_info[j].buffer_info.empty()) {
                    writes[j].descriptorCount = 1;
                    writes[j].pBufferInfo = &write_info[j].buffer_info[0];
                }
            }

            ctx->device.logical.updateDescriptorSets(writes, nullptr, ctx->dispatcher);
        }

    vk::DescriptorSet SingleDescriptorSet::handle() const {
        return descriptor_set;
    }
} // namespace caelus::core::api