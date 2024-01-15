#pragma once

#include "render/backend/factory/frame_buffer_factory.h"
#include "vulkan/vulkan.h"

class FrameBufferFactoryVulkan : public FrameBufferFactory {
public:
    FrameBufferFactoryVulkan(VkDevice logicalDevice);

    FrameBuffer* createFrameBufferSimple(RenderPass* renderPass, Image* attachment, u32 width, u32 height) override;

    void destroyFrameBuffer(FrameBuffer* frameBuffer) override;

private:
    VkDevice m_logicalDevice;
};