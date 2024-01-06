#pragma once

#include "render/backend/type/mysettings.h"

// This needs to be separate from the DescriptorSets are owned by the pools
// And these are owned by the device.

class DescriptorSetLayout {
public:
    virtual ~DescriptorSetLayout() = default;
};