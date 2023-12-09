#pragma once

#include "typedef.h"
#include "vulkan/vulkan.h"

class CommandBuffer {
public:
    CommandBuffer(VkDevice device, VkCommandPool commandPool);
    ~CommandBuffer();
    
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

    void begin();
    void end();
    void submit(VkQueue queue);
    void reset();

    void copyBuffer(VkBuffer src, VkBuffer dst, size_t size);
    void copyBufferToImage(VkBuffer src, VkImage dst, u32 width, u32 height);

    void transitionImageFormat(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

private:
    VkDevice m_device;
    VkCommandPool m_commandPool;
    VkCommandBuffer m_commandBuffer;
};