cmake_minimum_required(VERSION 3.15)
project(Caelus)

find_package(Vulkan REQUIRED)

add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(external/entt)

include_directories(Caelus
        include
        external/stb
        external/glm
        external/entt/src
        external/glfw/include
        external/VulkanMemoryAllocator/src
        ${Vulkan_INCLUDE_DIRS})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 20)

set(STB_HEADERS
        external/stb/stb.h
        external/stb/stb_image.h)

set(STB_SOURCES
        src/engine/STB_Impl.cpp)

set(VMA_HEADERS
        external/VulkanMemoryAllocator/src/vk_mem_alloc.h)

set(CAELUS_HEADERS
        ${STB_HEADERS}
        ${VMA_HEADERS}
        include/engine/Logger.hpp
        include/engine/Util.hpp
        include/engine/Types.hpp
        include/engine/core/Window.hpp
        include/engine/core/api/VulkanContext.hpp
        include/engine/Forwards.hpp
        include/engine/core/api/Instance.hpp
        include/engine/core/api/Device.hpp
        include/engine/core/api/Swapchain.hpp
        include/engine/core/api/CommandPool.hpp
        include/engine/core/api/CommandBuffer.hpp
        include/engine/core/api/DescriptorPool.hpp
        include/engine/core/api/RenderPass.hpp
        include/engine/core/api/Framebuffer.hpp
        include/engine/core/api/renderer/Renderer.hpp
        include/engine/core/api/VertexBuffer.hpp
        include/engine/core/api/Pipeline.hpp
        include/engine/core/Application.hpp
        include/engine/core/api/Buffer.hpp
        include/engine/core/api/DescriptorSet.hpp
        include/engine/Constants.hpp
        include/engine/core/api/MappedBuffer.hpp
        include/engine/core/components/Camera.hpp
        include/engine/core/Globals.hpp
        include/engine/core/components/Texture.hpp
        include/engine/core/api/Image.hpp
        include/engine/core/api/Sampler.hpp
        include/engine/core/components/Mesh.hpp
        include/engine/core/api/renderer/RenderGraph.hpp
        include/engine/core/components/Transform.hpp)

set(CAELUS_SOURCES
        ${STB_SOURCES}
        src/main.cpp
        src/engine/Util.cpp
        src/engine/core/Window.cpp
        src/engine/core/api/VulkanContext.cpp
        src/engine/core/api/Instance.cpp
        src/engine/core/api/Device.cpp
        src/engine/core/api/Swapchain.cpp
        src/engine/core/api/CommandPool.cpp
        src/engine/core/api/CommandBuffer.cpp
        src/engine/core/api/DescriptorPool.cpp
        src/engine/core/api/RenderPass.cpp
        src/engine/core/api/Framebuffer.cpp
        src/engine/core/api/renderer/Renderer.cpp
        src/engine/core/api/Pipeline.cpp
        src/engine/core/Application.cpp
        src/engine/core/api/VertexBuffer.cpp
        src/engine/core/api/Buffer.cpp
        src/engine/core/api/DescriptorSet.cpp
        src/engine/core/api/MappedBuffer.cpp
        src/engine/core/api/VulkanMemoryAllocator.cpp
        src/engine/core/components/Camera.cpp
        src/engine/core/api/Image.cpp
        src/engine/core/components/Texture.cpp
        src/engine/core/api/Sampler.cpp
        src/engine/core/components/Mesh.cpp)

add_executable(Caelus
        ${CAELUS_HEADERS}
        ${CAELUS_SOURCES})

if (UNIX)
    target_compile_options(Caelus PRIVATE -std=c++20 -O3 -pedantic -pedantic-errors -m64 -Wall -Wextra -Werror -Wno-c99-extensions -Wno-shadow -Wno-deprecated-declarations)
endif()

if (WIN32)
    target_compile_definitions(Caelus PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if (UNIX)
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_definitions(Caelus PRIVATE _GLIBCXX_DEBUG)
    endif()
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(Caelus PRIVATE CAELUS_DEBUG)
endif()

if (UNIX)
    set_target_properties(
            glfw
            PROPERTIES
            COMPILE_OPTIONS -Wno-everything)
endif()

if (WIN32)
    target_link_libraries(Caelus glfw)
elseif (UNIX)
    target_link_libraries(Caelus glfw pthread)
endif()