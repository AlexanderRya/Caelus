#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/Instance.hpp>
#include <engine/Logger.hpp>
#include <engine/Types.hpp>
#include <engine/Util.hpp>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace caelus::core::api {
    static std::string get_message_type(const VkDebugUtilsMessageTypeFlagsEXT& type) {
        switch (type) {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
                return "General";
            }

            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
                return "Validation";
            }

            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
                return "Performance";
            }

            default: {
                return "Unknown";
            }
        }
    }

    static std::string get_message_severity(const VkDebugUtilsMessageSeverityFlagBitsEXT& type) {
        switch (type) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
                return "Verbose";
            }

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
                return "Info";
            }

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                return "Warning";
            }

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                return "Error";
            }

            default: {
                return "Unknown";
            }
        }
    }

    [[maybe_unused]] VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void*) {

        util::print(util::format(
            "[{}] [Vulkan] [{}/{}]: {}\n",
            util::get_current_timestamp(),
            get_message_severity(message_severity),
            get_message_type(message_type),
            callback_data->pMessage));

        if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            throw std::runtime_error("Validation layer error");
        }

        return 0;
    }

    [[nodiscard]] static inline std::vector<const char*> get_required_extensions(const VulkanContext& ctx) {
        u32 count = 0;

        auto required_extensions = glfwGetRequiredInstanceExtensions(&count);
        auto extensions = vk::enumerateInstanceExtensionProperties(nullptr, {}, ctx.dispatcher);

        std::vector<const char*> enabled_extensions;
        enabled_extensions.reserve(count + 2);

        for (u32 i = 0; i < count; ++i) {
            for (const auto& extension : extensions) {
                if (std::strcmp(extension.extensionName, required_extensions[i]) == 0) {
                    logger::info("Required extension activated: ", enabled_extensions.emplace_back(required_extensions[i]));
                    break;
                }
            }
        }

        if (enabled_extensions.size() != count) {
            throw std::runtime_error("Required extension not supported.");
        }
#ifdef CAELUS_DEBUG
        enabled_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        return enabled_extensions;
    }

    vk::Instance make_instance(const VulkanContext& ctx) {
        vk::ApplicationInfo application_info{}; {
            application_info.apiVersion = VK_API_VERSION_1_2;
            application_info.applicationVersion = CAELUS_MAKE_VERSION(0u, 1u, 0u);
            application_info.engineVersion = CAELUS_MAKE_VERSION(0u, 1u, 0u);
            application_info.pEngineName = "Caelus";
            application_info.pApplicationName = "Caelus";
        }

        auto enabled_exts = get_required_extensions(ctx);

        [[maybe_unused]] const char* validation_layer = "VK_LAYER_KHRONOS_validation";

        vk::InstanceCreateInfo instance_create_info{}; {
            instance_create_info.pApplicationInfo = &application_info;

            instance_create_info.ppEnabledExtensionNames = enabled_exts.data();
            instance_create_info.enabledExtensionCount = enabled_exts.size();
#ifdef CAELUS_DEBUG
            instance_create_info.ppEnabledLayerNames = &validation_layer;
            instance_create_info.enabledLayerCount = 1;
#else
            instance_create_info.ppEnabledLayerNames = nullptr;
            instance_create_info.enabledLayerCount = 0;
#endif
        }

        return vk::createInstance(instance_create_info, nullptr, ctx.dispatcher);
    }

    vk::DebugUtilsMessengerEXT install_validation_layers(const VulkanContext& ctx) {
        vk::DebugUtilsMessengerCreateInfoEXT create_info{}; {
            create_info.messageSeverity =
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
            create_info.messageType =
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral    |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
            create_info.pfnUserCallback = vulkan_debug_callback;
        }
        
        return ctx.instance.createDebugUtilsMessengerEXT(create_info, nullptr, ctx.dispatcher);
    }
} // namespace caelus::core::api