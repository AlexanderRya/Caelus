#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/Pipeline.hpp>
#include <engine/Constants.hpp>
#include <engine/Logger.hpp>
#include <engine/Types.hpp>

#include <fstream>

namespace caelus::core::api {
    [[nodiscard]] static inline vk::ShaderModule load_module(const VulkanContext* ctx, const std::filesystem::path& path) {
        std::ifstream in(path.generic_string(), std::fstream::binary);

        if (!in.is_open()) {
            throw std::runtime_error("Error, \"" + path.filename().generic_string() + "\" file not found.");
        }

        std::string spv{ std::istreambuf_iterator<char>{ in }, {} };

        vk::ShaderModuleCreateInfo create_info{}; {
            create_info.codeSize = spv.size();
            create_info.pCode = reinterpret_cast<const u32*>(spv.data());
        }

        auto module = ctx->device.logical.createShaderModule(create_info, nullptr, ctx->dispatcher);

        logger::info("Module \"" + path.filename().generic_string() + "\" successfully loaded");

        return module;
    }

    PipelineLayout make_generic_pipeline_layout(const VulkanContext& ctx) {
        PipelineLayout layout;

        std::array<vk::DescriptorSetLayoutBinding, 4> layout_bindings{}; {
            layout_bindings[0].descriptorCount = 1;
            layout_bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
            layout_bindings[0].binding = static_cast<u32>(meta::PipelineBinding::eCamera);
            layout_bindings[0].stageFlags = vk::ShaderStageFlagBits::eVertex;

            layout_bindings[1].descriptorCount = 1;
            layout_bindings[1].descriptorType = vk::DescriptorType::eStorageBuffer;
            layout_bindings[1].binding = static_cast<u32>(meta::PipelineBinding::eInstance);
            layout_bindings[1].stageFlags = vk::ShaderStageFlagBits::eVertex;

            layout_bindings[2].descriptorCount = 1;
            layout_bindings[2].descriptorType = vk::DescriptorType::eUniformBuffer;
            layout_bindings[2].binding = static_cast<u32>(meta::PipelineBinding::eMaterial);
            layout_bindings[2].stageFlags = vk::ShaderStageFlagBits::eFragment;

            layout_bindings[3].descriptorCount = 128;
            layout_bindings[3].descriptorType = vk::DescriptorType::eCombinedImageSampler;
            layout_bindings[3].binding = static_cast<u32>(meta::PipelineBinding::eTexture);
            layout_bindings[3].stageFlags = vk::ShaderStageFlagBits::eFragment;
        }

        std::array<vk::DescriptorBindingFlags, 4> binding_flags{}; {
            binding_flags[0] = {};
            binding_flags[1] = {};
            binding_flags[2] = {};
            binding_flags[3] = vk::DescriptorBindingFlagBits::eVariableDescriptorCount | vk::DescriptorBindingFlagBits::ePartiallyBound;
        }

        vk::DescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info{}; {
            binding_flags_info.bindingCount = binding_flags.size();
            binding_flags_info.pBindingFlags = binding_flags.data();
        }

        vk::DescriptorSetLayoutCreateInfo set_layout_create_info{}; {
            set_layout_create_info.pNext = &binding_flags_info;
            set_layout_create_info.bindingCount = layout_bindings.size();
            set_layout_create_info.pBindings = layout_bindings.data();
        }

        layout.set = ctx.device.logical.createDescriptorSetLayout(set_layout_create_info, nullptr, ctx.dispatcher);

        vk::PipelineLayoutCreateInfo layout_create_info{}; {
            layout_create_info.setLayoutCount = 1;
            layout_create_info.pSetLayouts = &layout.set;
            layout_create_info.pushConstantRangeCount = 0;
            layout_create_info.pPushConstantRanges = nullptr;
        }

        layout.pipeline = ctx.device.logical.createPipelineLayout(layout_create_info, nullptr, ctx.dispatcher);

        return layout;
    }

    Pipeline make_generic_pipeline(const Pipeline::CreateInfo& info) {
        std::array<vk::ShaderModule, 2> modules{}; {
            modules[0] = load_module(info.ctx, info.vertex_path);
            modules[1] = load_module(info.ctx, info.fragment_path);
        }

        std::array<vk::PipelineShaderStageCreateInfo, 2> stages{}; {
            /* Vertex Module */ {
                vk::PipelineShaderStageCreateInfo vertex_stage{}; {
                    vertex_stage.pName = "main";
                    vertex_stage.module = modules[0];
                    vertex_stage.stage = vk::ShaderStageFlagBits::eVertex;
                }

                stages[0] = vertex_stage;
            }

            /* Fragment Module */ {
                vk::PipelineShaderStageCreateInfo fragment_stage{}; {
                    fragment_stage.pName = "main";
                    fragment_stage.module = modules[1];
                    fragment_stage.stage = vk::ShaderStageFlagBits::eFragment;
                }

                stages[1] = fragment_stage;
            }
        }

        constexpr std::array dynamic_states{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };

        vk::PipelineDynamicStateCreateInfo dynamic_state_create_info{}; {
            dynamic_state_create_info.dynamicStateCount = dynamic_states.size();
            dynamic_state_create_info.pDynamicStates = dynamic_states.data();
        }

        std::array<vk::VertexInputBindingDescription, 1> vertex_bindings{}; {
            vertex_bindings[0].stride = sizeof(Vertex);
            vertex_bindings[0].binding = 0;
            vertex_bindings[0].inputRate = vk::VertexInputRate::eVertex;
        }

        std::array<vk::VertexInputAttributeDescription, 3> vertex_attributes{}; {
            vertex_attributes[0].binding = 0;
            vertex_attributes[0].format = vk::Format::eR32G32B32Sfloat;
            vertex_attributes[0].location = 0;
            vertex_attributes[0].offset = offsetof(Vertex, pos);

            vertex_attributes[1].binding = 0;
            vertex_attributes[1].format = vk::Format::eR32G32B32Sfloat;
            vertex_attributes[1].location = 1;
            vertex_attributes[1].offset = offsetof(Vertex, normals);

            vertex_attributes[2].binding = 0;
            vertex_attributes[2].format = vk::Format::eR32G32Sfloat;
            vertex_attributes[2].location = 2;
            vertex_attributes[2].offset = offsetof(Vertex, uvs);
        }

        vk::PipelineVertexInputStateCreateInfo vertex_input_info{}; {
            vertex_input_info.pVertexBindingDescriptions = vertex_bindings.data();
            vertex_input_info.vertexBindingDescriptionCount = vertex_bindings.size();
            vertex_input_info.pVertexAttributeDescriptions = vertex_attributes.data();
            vertex_input_info.vertexAttributeDescriptionCount = vertex_attributes.size();
        }

        vk::PipelineInputAssemblyStateCreateInfo input_assembly{}; {
            input_assembly.topology = vk::PrimitiveTopology::eTriangleList;
            input_assembly.primitiveRestartEnable = false;
        }

        vk::Viewport viewport{};
        vk::Rect2D scissor{};

        vk::PipelineViewportStateCreateInfo viewport_state{}; {
            viewport_state.viewportCount = 1;
            viewport_state.pViewports = &viewport;
            viewport_state.scissorCount = 1;
            viewport_state.pScissors = &scissor;
        }

        vk::PipelineRasterizationStateCreateInfo rasterizer_state_info{}; {
            rasterizer_state_info.lineWidth = 1.0f;
            rasterizer_state_info.depthBiasEnable = false;
            rasterizer_state_info.depthClampEnable = false;
            rasterizer_state_info.rasterizerDiscardEnable = false;
            rasterizer_state_info.polygonMode = vk::PolygonMode::eFill;
            rasterizer_state_info.cullMode = vk::CullModeFlagBits::eNone;
            rasterizer_state_info.frontFace = vk::FrontFace::eCounterClockwise;
        }

        vk::PipelineMultisampleStateCreateInfo multisampling_state_info{}; {
            multisampling_state_info.alphaToCoverageEnable = false;
            multisampling_state_info.sampleShadingEnable = false;
            multisampling_state_info.alphaToOneEnable = false;
            multisampling_state_info.rasterizationSamples = vk::SampleCountFlagBits::e1;
            multisampling_state_info.minSampleShading = 1.0f;
            multisampling_state_info.pSampleMask = nullptr;
        }

        vk::PipelineDepthStencilStateCreateInfo depth_stencil_info{}; {
            depth_stencil_info.stencilTestEnable = false;
            depth_stencil_info.depthTestEnable = true;
            depth_stencil_info.depthWriteEnable = true;
            depth_stencil_info.depthCompareOp = vk::CompareOp::eLess;
            depth_stencil_info.depthBoundsTestEnable = false;
            depth_stencil_info.minDepthBounds = 0.0f;
            depth_stencil_info.maxDepthBounds = 1.0f;
            depth_stencil_info.stencilTestEnable = false;
            depth_stencil_info.front = vk::StencilOpState{};
            depth_stencil_info.back = vk::StencilOpState{};
        }

        vk::PipelineColorBlendAttachmentState color_blend_attachment{}; {
            color_blend_attachment.blendEnable = true;
            color_blend_attachment.colorWriteMask =
                vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eA;
            color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
            color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
            color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;
            color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
            color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
            color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;
        }

        vk::PipelineColorBlendStateCreateInfo color_blend_info{}; {
            color_blend_info.attachmentCount = 1;
            color_blend_info.pAttachments = &color_blend_attachment;
            color_blend_info.logicOp = vk::LogicOp::eCopy;
            color_blend_info.logicOpEnable = false;
            color_blend_info.blendConstants[0] = 0.0f;
            color_blend_info.blendConstants[1] = 0.0f;
            color_blend_info.blendConstants[2] = 0.0f;
            color_blend_info.blendConstants[3] = 0.0f;
        }

        vk::GraphicsPipelineCreateInfo pipeline_info{}; {
            pipeline_info.stageCount = stages.size();
            pipeline_info.pStages = stages.data();
            pipeline_info.pVertexInputState = &vertex_input_info;
            pipeline_info.pInputAssemblyState = &input_assembly;
            pipeline_info.pViewportState = &viewport_state;
            pipeline_info.pRasterizationState = &rasterizer_state_info;
            pipeline_info.pMultisampleState = &multisampling_state_info;
            pipeline_info.pDepthStencilState = &depth_stencil_info;
            pipeline_info.pColorBlendState = &color_blend_info;
            pipeline_info.pDynamicState = &dynamic_state_create_info;
            pipeline_info.layout = info.layout.pipeline;
            pipeline_info.renderPass = info.ctx->default_render_pass;
            pipeline_info.subpass = 0;
            pipeline_info.basePipelineHandle = nullptr;
            pipeline_info.basePipelineIndex = -1;
        }

        Pipeline pipeline{}; {
            pipeline.handle = info.ctx->device.logical.createGraphicsPipeline(nullptr, pipeline_info, nullptr, info.ctx->dispatcher);
            pipeline.layout = info.layout;
        }

        logger::info("Generic pipeline successfully created");

        for (const auto& module : modules) {
            info.ctx->device.logical.destroyShaderModule(module, nullptr, info.ctx->dispatcher);
        }

        return pipeline;
    }
} // namespace caelus::core::api