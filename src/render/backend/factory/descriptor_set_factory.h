#pragma once

#include "render/backend/type/descriptor_set.h"

class DescriptorSetFactory {
public:
    virtual ~DescriptorSetFactory() = default;

    virtual DescriptorSet* createDescriptorSet(const DescriptorSetLayout& layout) = 0;

    virtual void destroyDescriptorSet(DescriptorSet* shader) = 0;
};