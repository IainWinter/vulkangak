#pragma once

#include "render/backend/type/image_sampler.h"
#include "vulkan/vulkan.h"

class ImageSamplerVulkan : public ImageSampler {
public:
    VkSampler sampler;
};