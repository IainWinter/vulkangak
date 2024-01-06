#pragma once

#include "render/backend/type/descriptor_set_layout.h"
#include "vulkan/vulkan.h"

class DescriptorSetLayoutVulkan : public DescriptorSetLayout {
public:
    VkDescriptorSetLayout layout;
};