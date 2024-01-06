#pragma once

#include "render/backend/type/descriptor_set.h"
#include "vulkan/vulkan.h"

#include "render/backend/frame_sync.h"

class DescriptorSetVulkan : public DescriptorSet {
public:
    VkDevice logicalDevice;

    VkDescriptorSetLayout layout;
    std::vector<VkDescriptorSet> sets; // indexed on [frame]

    // allows for auto fill of frame index
    FrameSyncInfo frameSyncInfo;

    void writeImage(int bindingIndex, Image* image, ImageSampler* sampler) override;
    void writeUniformBuffer(int bindingIndex, Buffer* uniformBuffer) override;
};