#pragma once

#include "render/backend/type/descriptor_set.h"
#include "vulkan/vulkan.h"

class DescriptorSetVulkan : public DescriptorSet {
public:
    VkDevice logicalDevice;

    VkDescriptorSetLayout layout;
    std::vector<VkDescriptorSet> sets; // indexed on [frame]

    void writeImage(int frameIndex, int bindingIndex, Image* image, ImageSampler* sampler) override;
    void writeUniformBuffer(int frameIndex, int bindingIndex, Buffer* uniformBuffer) override;
};