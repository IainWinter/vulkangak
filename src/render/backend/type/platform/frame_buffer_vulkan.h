#pragma once

#include "render/backend/type/frame_buffer.h"
#include "vulkan/vulkan.h"

class FrameBufferVulkan : public FrameBuffer {
public:
    VkFramebuffer frameBuffer;
};