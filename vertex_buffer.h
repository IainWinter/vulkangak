#pragma once

#include "buffer.h"

class VertexBuffer : Buffer {
public:
    VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t vertexSize, const void* data, size_t vertexCount);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    
    void draw(VkCommandBuffer commandBuffer);

private:
    u32 m_vertexCount;
};