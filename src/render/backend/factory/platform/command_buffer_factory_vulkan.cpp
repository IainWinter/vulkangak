#pragma once

#include "render/backend/factory/platform/command_buffer_factory_vulkan.h"
#include "render/backend/type/platform/command_buffer_vulkan.h"
#include "render/backend/type/platform/vk_error.h"

CommandBufferFactoryVulkan::CommandBufferFactoryVulkan(VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue) 
    : m_logicalDevice (logicalDevice) 
    , m_commandPool   (commandPool)
    , m_graphicsQueue (graphicsQueue)
{}

CommandBufferFactoryVulkan::~CommandBufferFactoryVulkan() {
}

CommandBuffer* CommandBufferFactoryVulkan::createCommandBuffer() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer vkCommandBuffer;
    vk(vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &vkCommandBuffer));

    CommandBufferVulkan* commandBuffer = new CommandBufferVulkan();
    commandBuffer->commandBuffer = vkCommandBuffer;
    commandBuffer->queue = m_graphicsQueue;

    return commandBuffer;
}

void CommandBufferFactoryVulkan::destroyCommandBuffer(CommandBuffer* commandBuffer) {
    CommandBufferVulkan* commandBufferVulkan = (CommandBufferVulkan*)commandBuffer;
    
    vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &commandBufferVulkan->commandBuffer);
    delete commandBufferVulkan;
}