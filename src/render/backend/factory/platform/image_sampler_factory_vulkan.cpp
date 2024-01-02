#pragma once

#include "render/backend/factory/platform/image_sampler_factory_vulkan.h"
#include "render/backend/type/platform/vk_error.h"

ImageSamplerFactoryVulkan::ImageSamplerFactoryVulkan(VkDevice logicalDevice) 
    : m_logicalDevice (logicalDevice) 
{}

ImageSamplerFactoryVulkan::~ImageSamplerFactoryVulkan() {
}

ImageSampler* ImageSamplerFactoryVulkan::createImageSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16; // should pass this in, from deviceprops.limits.maxSamplerAnisotropy
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkSampler vkSampler;
    vk(vkCreateSampler(m_logicalDevice, &samplerInfo, nullptr, &vkSampler));

    ImageSamplerVulkan* imageSampler = new ImageSamplerVulkan();
    imageSampler->sampler = vkSampler;

    return imageSampler;
}

void ImageSamplerFactoryVulkan::destroyImageSampler(ImageSampler* image) {
    ImageSamplerVulkan* imageSampler = static_cast<ImageSamplerVulkan*>(image);
    
    vkDestroySampler(m_logicalDevice, imageSampler->sampler, nullptr);
    delete imageSampler;
}