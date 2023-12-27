#pragma once

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
        u32 swapchainImageCount
    );

    ~ImGuiLoop();

    ImGuiLoop(const ImGuiLoop&) = delete;
    ImGuiLoop& operator=(const ImGuiLoop&) = delete;

    void beginFrame();
    void submitFrame(VkCommandBuffer commandBuffer);

private:
    Window* m_window;
    VkDevice m_device;

    VkDescriptorPool m_pool;
};