#pragma once

#include "render/backend/type/descriptor_set.h"
#include "render/backend/frame_sync.h"
#include "vulkan/vulkan.h"

class DescriptorSetVulkan : public DescriptorSet {
public:
    VkDevice logicalDevice;

    std::vector<VkDescriptorSet> sets; // indexed on [frame]

    // allows for auto fill of frame index
    FrameSyncInfo frameSyncInfo;

    // could want to add layout here but im not sure if it is needed

    void writeImage(int bindingIndex, Image* image, ImageSampler* sampler) override;
    void writeUniformBuffer(int bindingIndex, Buffer* uniformBuffer) override;
};