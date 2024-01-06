#pragma once

#include "render/backend/factory/descriptor_set_layout_factory.h"
#include "vulkan/vulkan.h"

// This factory holds a single pool
class DescriptorSetLayoutFactoryVulkan : public DescriptorSetLayoutFactory {
public:
    DescriptorSetLayoutFactoryVulkan(VkDevice logicalDevice);
    ~DescriptorSetLayoutFactoryVulkan();

    DescriptorSetLayout* createDescriptorSetLayout(const DescriptorSetLayoutConfig& config) override;

    void destroyDescriptorSetLayout(DescriptorSetLayout* layout) override;

private:
    VkDevice m_logicalDevice;
};