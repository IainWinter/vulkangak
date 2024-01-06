#pragma once

#include "render/backend/type/command_buffer.h"
#include "render/backend/frame_sync.h"
#include "vulkan/vulkan.h"

class CommandBufferVulkan : public CommandBuffer {
public:
    VkCommandBuffer commandBuffer;
    VkQueue queue;

    // allows auto setting of frame index dependent arguments
    FrameSyncInfo frameSyncInfo;

    void begin() override;
    void end() override;
    void submit() override;
    void reset() override;

    void endRenderPass() override;

    void copyBuffer(Buffer* src, Buffer* dst, size_t size) override;
    void copyBufferToImage(Buffer* src, Image* dst, u32 width, u32 height) override;

    void transitionImageFormat(Image* image, ImageLayout oldLayout, ImageLayout newLayout) override;

    void setShader(Shader* shader) override;
    void pushConstant(Shader* shader, u32 index, const void* data) override;
    void bindDescriptorSet(Shader* shader, u32 setIndex, DescriptorSet* descriptorSet) override;

    void bindVertexBuffers(u32 count, Buffer* vertexBuffers[]) override;
    void bindIndexBuffer(Buffer* indexBuffer) override;

    void draw(u32 vertexCount, u32 instanceCount) override;
    void drawIndexed(u32 indexCount, u32 instanceCount) override;
};