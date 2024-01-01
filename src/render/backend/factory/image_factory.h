#pragma once

#include "render/backend/type/image.h"

#include "typedef.h"
#include "asset/image.h"

class ImageFactory {
public:
    virtual ~ImageFactory() = default;

    virtual Image* createImage2DEmpty(u32 width, u32 height, ImageFormat format) = 0;
    virtual Image* createImage2DEmptyMultisample(u32 width, u32 height, ImageFormat format, ImageSampleCount sampleCount) = 0;

    virtual Image* createImage2D(u32 width, u32 height, ImageFormat format, const u8* pixels) = 0;

    virtual Image* createImage2DFromAsset(const ImageAsset& asset) = 0;

    virtual void destroyImage(Image* image) = 0;
};