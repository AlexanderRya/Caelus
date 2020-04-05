#include <engine/core/api/renderer/ImGuiContext.hpp>
#include <engine/core/api/CommandBuffer.hpp>
#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/Window.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace caelus::core::api {
    ImGuiContext make_imgui_context(const VulkanContext& ctx, const Window& window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(window.handle(), true);

        ImGui_ImplVulkan_InitInfo info{}; {
            info.PhysicalDevice = ctx.device.physical;
            info.Instance = ctx.instance;
            info.Device = ctx.device.logical;
            info.Allocator = nullptr;
            info.CheckVkResultFn = nullptr;
            info.DescriptorPool = ctx.descriptor_pool;
            info.MinImageCount = ctx.swapchain.image_count - 1;
            info.ImageCount = ctx.swapchain.image_count - 1;
            info.Queue = ctx.device.queue;
            info.QueueFamily = ctx.device.queue_family;
            info.MSAASamples = {};
        }

        ImGui_ImplVulkan_Init(&info, ctx.default_render_pass, &ctx.dispatcher);

        auto command_buffer = begin_transient(ctx); {
            ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
        } end_transient(ctx, command_buffer);

        ImGui_ImplVulkan_DestroyFontUploadObjects();

        return {};
    }
} // namespace caelus::core::api