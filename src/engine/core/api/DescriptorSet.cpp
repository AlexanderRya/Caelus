#include <engine/core/api/MappedBuffer.hpp>
#include <engine/core/api/DescriptorSet.hpp>
#include <engine/core/api/VulkanContext.hpp>
#include <engine/Constants.hpp>
#include <engine/Logger.hpp>

namespace caelus::core::api {
    void DescriptorSet::create(const CreateInfo& info) {
        ctx = info.ctx;

        vk::DescriptorSetAllocateInfo allocate_info{}; {
            allocate_info.descriptorSetCount = 1;
            allocate_info.pSetLayouts = &info.layout;
            allocate_info.descriptorPool = ctx->descriptor_pool;
        }
        descriptor_sets.resize(meta::frames_in_flight);

        for (usize i = 0; i < meta::frames_in_flight; ++i) {
            descriptor_sets[i] = ctx->device.logical.allocateDescriptorSets(allocate_info, ctx->dispatcher).back();
        }
    }

    void DescriptorSet::write(const std::vector<WriteInfo>& write_info) {
        std::vector<vk::WriteDescriptorSet> writes;
        writes.resize(write_info.size());

        for (usize i = 0; i < meta::frames_in_flight; ++i) {
            for (usize j = 0; j < write_info.size(); ++j) {
                writes[j].descriptorType = write_info[j].type;
                writes[j].dstBinding = write_info[j].binding;
                writes[j].descriptorCount = 1;
                writes[j].dstArrayElement = 0;
                writes[j].dstSet = descriptor_sets[i];
                writes[j].pTexelBufferView = nullptr;
                if (!write_info[j].buffer_info.empty()) {
                    writes[j].pBufferInfo = &write_info[j].buffer_info[i];
                } else {
                    writes[j].pImageInfo = &write_info[j].image_info;
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
                    write.descriptorCount = 1;
                    write.dstArrayElement = 0;
                    write.dstSet = descriptor_sets[i];
                    write.pTexelBufferView = nullptr;
                if (!write_info.buffer_info.empty()) {
                    write.pBufferInfo = &write_info.buffer_info[i];
                } else {
                    write.pImageInfo = &write_info.image_info;
                }
            }

            ctx->device.logical.updateDescriptorSets(write, nullptr, ctx->dispatcher);
        }
    }

    void DescriptorSet::write_at(const usize idx, const DescriptorSet::WriteInfo& write_info) {
        vk::WriteDescriptorSet write{}; {
            write.descriptorType = write_info.type;
            write.dstBinding = write_info.binding;
            write.descriptorCount = 1;
            write.dstArrayElement = 0;
            write.dstSet = descriptor_sets[idx];
            write.pTexelBufferView = nullptr;
            if (!write_info.buffer_info.empty()) {
                write.pBufferInfo = &write_info.buffer_info[0];
            } else {
                write.pImageInfo = &write_info.image_info;
            }
        }

        ctx->device.logical.updateDescriptorSets(write, nullptr, ctx->dispatcher);
    }

    vk::DescriptorSet DescriptorSet::operator [](const usize idx) const {
        return descriptor_sets[idx];
    }
} // namespace caelus::core::api