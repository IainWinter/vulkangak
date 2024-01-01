#pragma once

#include "render/backend/factory/image_sampler_factory.h"
#include "render/backend/type/platform/image_sampler_vulkan.h"

class ImageSamplerFactoryVulkan : public ImageSamplerFactory {
public:
    ImageSamplerFactoryVulkan(VkDevice logicalDevice);
    ~ImageSamplerFactoryVulkan();

    ImageSampler* createImageSampler() override;

    void destroyImageSampler(ImageSampler* image) override;

private:
    VkDevice m_logicalDevice;
};