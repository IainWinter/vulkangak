#pragma once

#include "render/backend/type/mysettings.h"
#include "vulkan/vulkan.h"

struct VulkanImageFormat {
    size_t pixelSize;
    size_t elementSize;
    VkFormat format;
};

VkShaderStageFlagBits shaderStageFlagMapVulkan(ShaderStageBit stages);
VkBufferUsageFlagBits bufferTypeMapVulkan(BufferType type);
VkDescriptorType descriptorTypeMapVulkan(DescriptorType type);
VulkanImageFormat imageFormatMapVulkan(ImageFormat format);
VkSampleCountFlagBits imageSampleCountMapVulkan(ImageSampleCount sampleCount);
VkFormat attributeFormatMapVulkan(AttributeFormat format);
VkImageLayout imageLayoutMapVulkan(ImageLayout layout);