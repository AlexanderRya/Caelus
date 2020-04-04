#ifndef CAELUS_FORWARDS_HPP
#define CAELUS_FORWARDS_HPP

struct GLFWwindow;

namespace vk {
    class Instance;
    class DebugUtilsMessengerEXT;
    class SurfaceKHR;
    class DescriptorPool;
    class CommandPool;
    class CommandBuffer;
    class RenderPass;
    class Framebuffer;
    class Fence;
    class Semaphore;
    class Sampler;
} // namespace vk

namespace caelus::core {
    class Window;
} // namespace caelus::core

namespace caelus::core::api {
    struct VulkanContext;
    struct Device;
    struct Swapchain;
    class DescriptorSet;
    struct Image;
} // namespace caelus::core::api

namespace caelus::core::api {
    class MappedBuffer;
    class SingleMappedBuffer;
} // namespace caelus::core::api

namespace caelus::core::components {
    struct Material;
    class Texture;
    class Camera;
    struct Mesh;
    struct Transform;
} // namespace caelus::core::components

#endif //CAELUS_FORWARDS_HPP
