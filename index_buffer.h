#pragma once

#include "buffer.h"

class IndexBuffer : Buffer {
public:
    IndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t indexCount, const u8* data);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

private:
    u32 m_indexCount;
};