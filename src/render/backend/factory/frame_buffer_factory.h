#pragma once

#include "render/backend/type/frame_buffer.h"
#include "render/backend/type/render_pass.h"
#include "render/backend/type/image.h"

class FrameBufferFactory {
public:
    virtual ~FrameBufferFactory() = default;

    virtual FrameBuffer* createFrameBufferSimple(RenderPass* renderPass, Image* attachment, u32 width, u32 height) = 0;

    virtual void destroyFrameBuffer(FrameBuffer* frameBuffer) = 0;
};