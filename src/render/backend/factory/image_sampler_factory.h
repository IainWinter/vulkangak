#pragma once

#include "render/backend/type/image_sampler.h"
#include "typedef.h"

class ImageSamplerFactory {
public:
    virtual ~ImageSamplerFactory() = default;

    virtual ImageSampler* createImageSampler() = 0;

    virtual void destroyImageSampler(ImageSampler* image) = 0;
};