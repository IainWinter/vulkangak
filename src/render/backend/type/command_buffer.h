#pragma once

#include "render/backend/type/shader.h"
#include "render/backend/type/image.h"
#include "render/backend/type/buffer.h"
#include "render/backend/type/descriptor_set.h"

class CommandBuffer {
public:
    virtual ~CommandBuffer() = default;

    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void submit() = 0;
    virtual void reset() = 0;

    virtual void endRenderPass() = 0;

    virtual void copyBuffer(Buffer* src, Buffer* dst, size_t size) = 0;
    virtual void copyBufferToImage(Buffer* src, Image* dst, u32 width, u32 height) = 0;

    virtual void transitionImageFormat(Image* image, ImageLayout oldLayout, ImageLayout newLayout) = 0;

    virtual void setShader(Shader* shader) = 0;
    virtual void pushConstant(Shader* shader, u32 index, const void* data) = 0;
    virtual void bindDescriptorSet(Shader* shader, u32 setIndex, DescriptorSet* descriptorSet) = 0;

    virtual void bindVertexBuffers(u32 count, Buffer* vertexBuffers[]) = 0;
    virtual void bindIndexBuffer(Buffer* indexBuffer) = 0;

    virtual void draw(u32 vertexCount, u32 instanceCount) = 0;
    virtual void drawIndexed(u32 indexCount, u32 instanceCount) = 0;
};