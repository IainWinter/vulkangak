#pragma once

#include "window.h"
#include "imgui_loop.h"

#include "vk_mem_alloc.h"

#include <vector>

#include "factory/platform/buffer_factory_vulkan.h"
#include "factory/platform/command_buffer_factory_vulkan.h"
#include "factory/platform/descriptor_set_factory_vulkan.h"
#include "factory/platform/descriptor_set_layout_factory_vulkan.h"
#include "factory/platform/image_factory_vulkan.h"
#include "factory/platform/image_sampler_factory_vulkan.h"
#include "factory/platform/shader_factory_vulkan.h"

struct VulkanSwapChainImage {
    VkImage image;
    VkImageView view;
    VkFramebuffer framebuffer;
};

struct VulkanFrame {
    CommandBuffer* commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
};

struct VulkanFrameImage {
    CommandBuffer* commandBuffer;
    u32 width;
    u32 height;

    u32 frameIndex;
    u32 imageIndex;
};

class RenderDevice {
public:
    RenderDevice(Window* window, bool useDebug);
    ~RenderDevice();

    RenderDevice(const RenderDevice&) = delete;
    RenderDevice& operator=(const RenderDevice&) = delete;

    FrameSyncInfo getFrameSyncInfo();

    ImGuiLoop* newImGuiLoop();

    void updateDescriptorSet(const VkWriteDescriptorSet& write);

    // Block until a frame should be drawn
    // Return true if the frame should be drawn
    bool waitBeginFrame(VulkanFrameImage* frame);

    // Submit the current frame, this should only be called if waitBeginFrame returns true
    void submitFrame();

    // When drawing to the screen, use this to get the render pass which draws to the screen
    // on the current frames command buffer
    void beginScreenRenderPass();

    // MUST call before destroying most resources
    void waitUntilIdle();

    void waitUntilGraphicsQueueIdle();

private:
    // Need a system to allocate more sets when needed
    // this prob crashes the system while running

    // void destroyDescriptorSets();

    // These functions are annoying because they init the m_ vars,
    // but are needed because the swap chain could need to change its config.
    // Their order is the order they should be called

    void chooseSurfaceConfig();
    void createPresentRenderPass();
    void createSwapchain();
    void destroySwapchain();

    // destroy, choose config, then create again
    void recreateSwapchain();

public:
    BufferFactory* bufferFactory;
    CommandBufferFactory* commandBufferFactory;
    DescriptorSetFactory* descriptorSetFactory;
    DescriptorSetLayoutFactory* descriptorSetLayoutFactory;
    ImageFactory* imageFactory;
    ImageSamplerFactory* imageSamplerFactory;
    ShaderFactory* shaderFactory;

private:
    Window* m_window;
    bool m_useDebug;

    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;

    VkSurfaceKHR m_surface;

    VkPhysicalDevice m_physicalDevice;
    u32 m_graphicsQueueIndex;
    u32 m_presentQueueIndex;

    VkDevice m_logicalDevice;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    // SwapChain config
    u32 m_minImageCount;
    VkSurfaceTransformFlagBitsKHR m_swapChainTransform;
    VkSurfaceFormatKHR m_format;
    VkPresentModeKHR m_present;
    VkExtent2D m_extent;

    VkSwapchainKHR m_swapChain;
    std::vector<VulkanSwapChainImage> m_images;

    // RenderPass stuff, this is the render pass for drawing to the screen
    // should this be a RenderPass?
    VkRenderPass m_renderPass;

    // Command pools, could put in a vector for each thread ID
    // for now, just store one
    VkCommandPool m_commandPool;

    // Frames in flight, this is the double buffer / triple buffer
    const u32 m_framesInFlight = 2;
    std::vector<VulkanFrame> m_frames;

    u32 m_currentFrameIndex = 0;
    u32 m_currentImageIndex = 0;

    VmaAllocator m_allocator;
};