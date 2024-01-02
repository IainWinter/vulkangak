#pragma once

#include "render/backend/type/platform/buffer_vulkan.h"
#include "render/backend/type/platform/command_buffer_vulkan.h"
#include "render/backend/type/platform/descriptor_set_vulkan.h"
#include "render/backend/type/platform/image_vulkan.h"
#include "render/backend/type/platform/shader_vulkan.h"
#include "render/backend/type/platform/translation_vulkan.h"
#include "render/backend/vk_error.h"

void CommandBufferVulkan::begin() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vk(vkBeginCommandBuffer(commandBuffer, &beginInfo));
}

void CommandBufferVulkan::end() {
    vk(vkEndCommandBuffer(commandBuffer));
}

void CommandBufferVulkan::submit() {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vk(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));

    // maybe put this in a waitSubmit() function
    vk(vkQueueWaitIdle(queue));
}

void CommandBufferVulkan::reset() {
    vk(vkResetCommandBuffer(commandBuffer, 0));
}

void CommandBufferVulkan::endRenderPass() {
    vkCmdEndRenderPass(commandBuffer);
}

void CommandBufferVulkan::copyBuffer(Buffer* src, Buffer* dst, size_t size) {
    BufferVulkan* vkSrc = (BufferVulkan*)src;
    BufferVulkan* vkDst = (BufferVulkan*)dst;

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, vkSrc->buffer, vkDst->buffer, 1, &copyRegion);
}

void CommandBufferVulkan::copyBufferToImage(Buffer* src, Image* dst, u32 width, u32 height) {
    BufferVulkan* vkSrc = (BufferVulkan*)src;
    ImageVulkan* vkDst = (ImageVulkan*)dst;

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

    vkCmdCopyBufferToImage(commandBuffer, vkSrc->buffer, vkDst->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void CommandBufferVulkan::transitionImageFormat(Image* image, ImageLayout oldLayout, ImageLayout newLayout) {
    ImageVulkan* vkImage = (ImageVulkan*)image;
    VkImageLayout vkOldLayout = imageLayoutMapVulkan(oldLayout);
    VkImageLayout vkNewLayout = imageLayoutMapVulkan(newLayout);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = vkOldLayout;
    barrier.newLayout = vkNewLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = vkImage->image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (   vkOldLayout == VK_IMAGE_LAYOUT_UNDEFINED 
        && vkNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } 
    
    else 
    if (   vkOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 
        && vkNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } 

    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void CommandBufferVulkan::setShader(Shader* shader) {
    ShaderVulkan* vkShader = static_cast<ShaderVulkan*>(shader);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkShader->pipeline);
}

void CommandBufferVulkan::pushConstant(Shader* shader, u32 index, const void* data) {
    ShaderVulkan* vkShader = static_cast<ShaderVulkan*>(shader);
    VkPushConstantRange pushConstant = vkShader->pushConstants.at(index);
    vkCmdPushConstants(commandBuffer, vkShader->pipelineLayout, pushConstant.stageFlags, pushConstant.offset, pushConstant.size, data);
}

void CommandBufferVulkan::bindDescriptorSet(Shader* shader, DescriptorSet* descriptorSet, u32 frameIndex) {
    ShaderVulkan* vkShader = static_cast<ShaderVulkan*>(shader);
    DescriptorSetVulkan* vkDescriptorSet = static_cast<DescriptorSetVulkan*>(descriptorSet);

    VkDescriptorSet set = vkDescriptorSet->sets[frameIndex];

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkShader->pipelineLayout, 0, 1, &set, 0, nullptr);
}

void CommandBufferVulkan::bindVertexBuffers(u32 count, Buffer* vertexBuffers[]) {
    VkBuffer buffers[4];
    VkDeviceSize offsets[4];
    
    for (u32 i = 0; i < count; i++) {
        BufferVulkan* vkBuffer = (BufferVulkan*)vertexBuffers[i];
        buffers[i] = vkBuffer->buffer;
        offsets[i] = 0;
    }

    vkCmdBindVertexBuffers(commandBuffer, 0, count, buffers, offsets);
}

void CommandBufferVulkan::bindIndexBuffer(Buffer* indexBuffer) {
    BufferVulkan* indexBufferVulkan = (BufferVulkan*)indexBuffer;

    vkCmdBindIndexBuffer(commandBuffer, indexBufferVulkan->buffer, 0, VK_INDEX_TYPE_UINT32);
}

void CommandBufferVulkan::draw(u32 vertexCount, u32 instanceCount) {
    vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);
}

void CommandBufferVulkan::drawIndexed(u32 indexCount, u32 instanceCount) {
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, 0, 0, 0);
}