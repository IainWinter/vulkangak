#pragma once

#include "render/backend/factory/command_buffer_factory.h"
#include "vulkan/vulkan.h"

// for now just pass queue to factory
// this only allows one type of queue per command factory which is dumb
// but I only use graphics queue so whatevs

class CommandBufferFactoryVulkan : public CommandBufferFactory {
public:
    CommandBufferFactoryVulkan(VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
    ~CommandBufferFactoryVulkan();

    CommandBuffer* createCommandBuffer() override;

    void destroyCommandBuffer(CommandBuffer* commandBuffer) override;

private:
    VkDevice m_logicalDevice;
    VkCommandPool m_commandPool;
    VkQueue m_graphicsQueue;
};