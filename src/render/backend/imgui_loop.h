#pragma once

#include "render/backend/factory/command_buffer_factory.h"

#include "typedef.h"
#include "vulkan/vulkan.h"

#include "window.h"

class ImGuiLoop {
public:
    ImGuiLoop(
        Window* window,
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice logicalDevice,
        VkRenderPass renderPass,
        VkCommandPool commandPool,
        VkQueue graphicsQueue,
        u32 graphicsQueueFamily,
        u32 swapchainImageCount,

        CommandBufferFactory* commandBufferFactory
    );

    ~ImGuiLoop();

    ImGuiLoop(const ImGuiLoop&) = delete;
    ImGuiLoop& operator=(const ImGuiLoop&) = delete;

    void beginFrame();
    void submitFrame(CommandBuffer* commandBuffer);

private:
    Window* m_window;
    VkDevice m_device;

    VkDescriptorPool m_pool;
};