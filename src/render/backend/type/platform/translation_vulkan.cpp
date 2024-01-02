#include "render/backend/type/platform/translation_vulkan.h"

static VkBufferUsageFlagBits s_bufferTypeMap[] = {
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
};

static VkDescriptorType s_descriptorTypeMap[] = {
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
};

static VulkanImageFormat s_imageFormatMap[] = {
    { 8, 24, VK_FORMAT_R8G8B8_SRGB },
    { 8, 24, VK_FORMAT_R8G8B8_UNORM },
    { 8, 32, VK_FORMAT_R8G8B8A8_SRGB },
    { 8, 32, VK_FORMAT_R8G8B8A8_UNORM }
};

static VkSampleCountFlagBits s_imageSampleCountMap[] = {
    VK_SAMPLE_COUNT_1_BIT,
    VK_SAMPLE_COUNT_2_BIT,
    VK_SAMPLE_COUNT_4_BIT,
    VK_SAMPLE_COUNT_8_BIT,
    VK_SAMPLE_COUNT_16_BIT,
    VK_SAMPLE_COUNT_32_BIT,
    VK_SAMPLE_COUNT_64_BIT
};

static VkFormat s_formatMap[] = {
    VK_FORMAT_R32_SFLOAT,
    VK_FORMAT_R32G32_SFLOAT,
    VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32B32A32_SFLOAT,
};

static VkImageLayout s_imageLayoutMap[] = {
    VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
};

VkShaderStageFlagBits shaderStageFlagMapVulkan(ShaderStageBit stages) {
    u32 bits = 0;
    if (stages & ShaderStage_Vertex)   bits |= VK_SHADER_STAGE_VERTEX_BIT;
    if (stages & ShaderStage_Fragment) bits |= VK_SHADER_STAGE_FRAGMENT_BIT;

    return (VkShaderStageFlagBits)bits;
}

VkBufferUsageFlagBits bufferTypeMapVulkan(BufferType type) {
    return s_bufferTypeMap[type];
}

VkDescriptorType descriptorTypeMapVulkan(DescriptorType type) {
    return s_descriptorTypeMap[type];
}

VulkanImageFormat imageFormatMapVulkan(ImageFormat format) {
    return s_imageFormatMap[format];
}

VkSampleCountFlagBits imageSampleCountMapVulkan(ImageSampleCount sampleCount) {
    return s_imageSampleCountMap[sampleCount];
}

VkFormat attributeFormatMapVulkan(AttributeFormat format) {
    return s_formatMap[format];
}

VkImageLayout imageLayoutMapVulkan(ImageLayout format) {
    return s_imageLayoutMap[format];
}