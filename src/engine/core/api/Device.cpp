#include <engine/core/api/VulkanContext.hpp>
#include <engine/core/api/Device.hpp>
#include <engine/Logger.hpp>
#include <engine/Types.hpp>

#include <vulkan/vulkan.hpp>

namespace caelus::core::api {
    [[nodiscard]] static inline vk::PhysicalDevice get_physical_device(const VulkanContext& ctx) {
        auto physical_devices = ctx.instance.enumeratePhysicalDevices(ctx.dispatcher);

        for (const auto& device : physical_devices) {
            auto device_properties = device.getProperties(ctx.dispatcher);
            auto device_features = device.getFeatures(ctx.dispatcher);


            if ((device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu  ||
                device_properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu ||
                device_properties.deviceType == vk::PhysicalDeviceType::eVirtualGpu) &&
                device_features.samplerAnisotropy &&
                device_features.multiDrawIndirect){

                logger::info("Selected physical device: ", device_properties.deviceName);
                return device;
            }
        }

        throw std::runtime_error("No suitable physical device available");
    }

    [[nodiscard]] static inline u32 get_queue_family(const vk::SurfaceKHR& surface, const vk::PhysicalDevice& physical_device, const vk::DispatchLoaderDynamic& dispatcher) {
        auto queue_family_properties = physical_device.getQueueFamilyProperties({}, dispatcher);

        for (u32 i = 0; i < queue_family_properties.size(); ++i) {
            if (((queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics) &&
                physical_device.getSurfaceSupportKHR(i, surface, dispatcher)) {
                return i;
            }
        }

        throw std::runtime_error("Failed to find a queue family");
    }

    [[nodiscard]] static inline vk::Device get_device(const u32 queue_family, const vk::PhysicalDevice& physical_device, const vk::DispatchLoaderDynamic& dispatcher) {
        auto extensions = physical_device.enumerateDeviceExtensionProperties(nullptr, {}, dispatcher);

        constexpr const char* enabled_exts[]{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        if (std::find_if(extensions.begin(), extensions.end(), [](const vk::ExtensionProperties& properties) {
            return std::strcmp(properties.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0;
        }) != extensions.end()) {
            float priorities[]{ 1.0f };

            vk::DeviceQueueCreateInfo queue_create_info{}; {
                queue_create_info.queueCount = 1;
                queue_create_info.queueFamilyIndex = queue_family;
                queue_create_info.pQueuePriorities = priorities;
            }

            vk::PhysicalDeviceFeatures features{}; {
                features.samplerAnisotropy = true;
                features.multiDrawIndirect = true;
            }

            vk::PhysicalDeviceDescriptorIndexingFeatures descriptor_indexing_features{}; {
                descriptor_indexing_features.shaderSampledImageArrayNonUniformIndexing = true;
                descriptor_indexing_features.descriptorBindingVariableDescriptorCount = true;
                descriptor_indexing_features.descriptorBindingPartiallyBound = true;
                descriptor_indexing_features.runtimeDescriptorArray = true;
            }

            vk::DeviceCreateInfo device_create_info{}; {
                device_create_info.pNext = &descriptor_indexing_features;
                device_create_info.ppEnabledExtensionNames = enabled_exts;
                device_create_info.enabledExtensionCount = 1;
                device_create_info.pQueueCreateInfos = &queue_create_info;
                device_create_info.queueCreateInfoCount = 1;
                device_create_info.pEnabledFeatures = &features;
            }

            return physical_device.createDevice(device_create_info, nullptr, dispatcher);
        } else {
            throw std::runtime_error("Selected physical device does not support a swapchain");
        }
    }

    [[nodiscard]] static inline vk::Queue get_queue(const vk::Device& device, const u32 queue_family, const vk::DispatchLoaderDynamic& dispatcher) {
        return device.getQueue(queue_family, 0, dispatcher);
    }

    Device make_device(const VulkanContext& ctx) {
        Device device{};

        device.physical = get_physical_device(ctx);
        device.queue_family = get_queue_family(ctx.surface, device.physical, ctx.dispatcher);
        device.logical = get_device(device.queue_family, device.physical, ctx.dispatcher);
        device.queue = get_queue(device.logical, device.queue_family, ctx.dispatcher);

        return device;
    }

    u64 find_memory_type(const VulkanContext& ctx, const u32 mask, const vk::MemoryPropertyFlags& flags) {
        const vk::PhysicalDeviceMemoryProperties memory_properties = ctx.device.physical.getMemoryProperties(ctx.dispatcher);

        for (u32 i = 0; i < memory_properties.memoryTypeCount; ++i) {
            if ((mask & (1u << i)) &&
                (memory_properties.memoryTypes[i].propertyFlags & flags) == flags) {
                return i;
            }
        }

        throw std::runtime_error("Failed to find memory type");
    }
} // namespace caelus::core::api