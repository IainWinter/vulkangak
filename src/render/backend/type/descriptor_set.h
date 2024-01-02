#pragma once

#include "render/backend/type/image.h"
#include "render/backend/type/image_sampler.h"
#include "render/backend/type/buffer.h"

class DescriptorSet {
public:
    virtual ~DescriptorSet() = default;

    virtual void writeImage(int frameIndex, int bindingIndex, Image* image, ImageSampler* sampler) = 0;
    virtual void writeUniformBuffer(int frameIndex, int bindingIndex, Buffer* uniformBuffer) = 0;
};