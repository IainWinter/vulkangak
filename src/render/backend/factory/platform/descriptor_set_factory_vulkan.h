#pragma once

#include "render/backend/factory/descriptor_set_factory.h"
#include "render/backend./frame.h"
#include "vulkan/vulkan.h"

// This factory holds a single pool
class DescriptorSetFactoryVulkan : public DescriptorSetFactory {
public:
    DescriptorSetFactoryVulkan(VkDevice logicalDevice, const DescriptorPoolLayout& poolLayout, const FrameSyncInfo& frameSyncInfo);
    ~DescriptorSetFactoryVulkan();

    DescriptorSet* createDescriptorSet(const DescriptorSetLayout& layout) override;

    void destroyDescriptorSet(DescriptorSet* descriptorSet) override;

private:
    VkDevice m_logicalDevice;
    FrameSyncInfo m_frameSyncInfo;

    VkDescriptorPool m_pool;
};