#pragma once

#include "render/backend/type/descriptor_set_layout.h"

class DescriptorSetLayoutFactory {
public:
    virtual ~DescriptorSetLayoutFactory() = default;

    virtual DescriptorSetLayout* createDescriptorSetLayout(const DescriptorSetLayoutConfig& config) = 0;

    virtual void destroyDescriptorSetLayout(DescriptorSetLayout* layout) = 0;
};