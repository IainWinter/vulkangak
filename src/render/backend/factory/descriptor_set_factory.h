#pragma once

#include "render/backend/type/descriptor_set.h"
#include "render/backend/type/descriptor_set_layout.h"

class DescriptorSetFactory {
public:
    virtual ~DescriptorSetFactory() = default;

    virtual DescriptorSet* createDescriptorSet(DescriptorSetLayout* layout) = 0;

    virtual void destroyDescriptorSet(DescriptorSet* shader) = 0;
};