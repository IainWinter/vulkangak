#pragma once

#include "buffer.h"

class IndexBuffer : public Buffer {
public:
    IndexBuffer(VmaAllocator allocator, size_t indexCount, const u8* data);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

public:
    u32 m_indexCount;
};