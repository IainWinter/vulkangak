#include "render/backend/type/platform/descriptor_set_vulkan.h"

#include "render/backend/type/platform/buffer_vulkan.h"
#include "render/backend/type/platform/image_vulkan.h"
#include "render/backend/type/platform/image_sampler_vulkan.h"

void DescriptorSetVulkan::writeImage(int bindingIndex, Image* image, ImageSampler* sampler) {
    ImageVulkan* imageVulkan = (ImageVulkan*)image;
    ImageSamplerVulkan* samplerVulkan = (ImageSamplerVulkan*)sampler;
    
    VkDescriptorSet set = sets.at(frameSyncInfo.frameIndex());

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageVulkan->view;
    imageInfo.sampler = samplerVulkan->sampler;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set;
    write.dstBinding = bindingIndex;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(logicalDevice, 1, &write, 0, nullptr);
}

void DescriptorSetVulkan::writeUniformBuffer(int bindingIndex, Buffer* uniformBuffer) {
    BufferVulkan* uniformBufferVulkan = (BufferVulkan*)uniformBuffer;

    VkDescriptorSet set = sets.at(frameSyncInfo.frameIndex());

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBufferVulkan->buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = uniformBufferVulkan->bufferSize;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set;
    write.dstBinding = bindingIndex;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(logicalDevice, 1, &write, 0, nullptr);
}