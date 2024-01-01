#pragma once

#include "typedef.h"

#include "render/backend/type/image_sampler.h"

class ImageSamplerFactory {
public:
    virtual ~ImageSamplerFactory() = default;

    virtual ImageSampler* createImageSampler() = 0;

    virtual void destroyImageSampler(ImageSampler* image) = 0;
};