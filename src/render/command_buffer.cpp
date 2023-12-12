#include "command_buffer.h"
#include "vk_error.h"

#include "shader.h"
#include "vertex_buffer.h"
#include "index_buffer.h"

#include <stdexcept>

// helpers

struct DrawCall {
    std::vector<VkBuffer> buffers;
    std::vector<VkDeviceSize> offsets;
};

DrawCall unrollDrawCall(const std::vector<VertexBuffer*>& vertexBuffers) {
    std::vector<VkBuffer> buffers;
    std::vector<VkDeviceSize> offsets;

    for (VertexBuffer* vertexBuffer : vertexBuffers) {
        buffers.push_back(vertexBuffer->m_buffer);
        offsets.push_back(0);
    }

    return { buffers, offsets };
}

CommandBuffer::CommandBuffer(VkDevice device, VkCommandPool commandPool) 
    : m_device      (device)
    , m_commandPool (commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    vk(vkAllocateCommandBuffers(device, &allocInfo, &m_commandBuffer));
}

CommandBuffer::~CommandBuffer() {
    if (m_device == VK_NULL_HANDLE) {
        return;
    }
    
    vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_commandBuffer);
}

void CommandBuffer::begin() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vk(vkBeginCommandBuffer(m_commandBuffer, &beginInfo));
}

void CommandBuffer::end() {
    vk(vkEndCommandBuffer(m_commandBuffer));
}

void CommandBuffer::submit(VkQueue queue) {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffer;

    vk(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));

    // maybe put this in a waitSubmit() function
    vk(vkQueueWaitIdle(queue));
}

void CommandBuffer::reset() {
    vk(vkResetCommandBuffer(m_commandBuffer, 0));
}

void CommandBuffer::copyBuffer(VkBuffer src, VkBuffer dst, size_t size) {
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(m_commandBuffer, src, dst, 1, &copyRegion);
}

void CommandBuffer::copyBufferToImage(VkBuffer src, VkImage dst, u32 width, u32 height) {
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(m_commandBuffer, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void CommandBuffer::transitionImageFormat(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } 
    
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } 

    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(m_commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void CommandBuffer::setShader(Shader* shader) {
    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->m_pipeline);
}

void CommandBuffer::pushConstant(Shader* shader, u32 index, const void* data) {
    VulkanPushConstant constant = shader->getPushConstant(index);
    vkCmdPushConstants(m_commandBuffer, shader->m_pipelineLayout, constant.stages, 0, constant.size, data);
}

void CommandBuffer::bindDescriptorSet(Shader* shader, VkDescriptorSet descriptorSet) {
    vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->m_pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);    
}

void CommandBuffer::draw(const std::vector<VertexBuffer*>& vertexBuffers) {
    drawInstanced(1, vertexBuffers);
}

void CommandBuffer::draw(IndexBuffer* indexBuffer, const std::vector<VertexBuffer*>& vertexBuffers) {
    drawInstanced(1, indexBuffer, vertexBuffers);
}

void CommandBuffer::drawInstanced(int instanceCount, const std::vector<VertexBuffer*>& vertexBuffers) {
    DrawCall drawCall = unrollDrawCall(vertexBuffers);
    vkCmdBindVertexBuffers(m_commandBuffer, 0, drawCall.buffers.size(), drawCall.buffers.data(), drawCall.offsets.data());
    vkCmdDraw(m_commandBuffer, vertexBuffers.at(0)->m_vertexCount, instanceCount, 0, 0);
}

void CommandBuffer::drawInstanced(int instanceCount, IndexBuffer* indexBuffer, const std::vector<VertexBuffer*>& vertexBuffers) {
    DrawCall drawCall = unrollDrawCall(vertexBuffers);
    vkCmdBindVertexBuffers(m_commandBuffer, 0, drawCall.buffers.size(), drawCall.buffers.data(), drawCall.offsets.data());
    vkCmdBindIndexBuffer(m_commandBuffer, indexBuffer->m_buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(m_commandBuffer, indexBuffer->m_indexCount, instanceCount, 0, 0, 0);
}