#pragma once

#include "buffer.h"

class IndexBuffer : public Buffer {
public:
    IndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t indexCount, const u8* data);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

public:
    u32 m_indexCount;
};