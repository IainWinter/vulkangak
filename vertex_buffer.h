#pragma once

#include "buffer.h"

class VertexBuffer : public Buffer {
public:
    VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t vertexSize, size_t vertexCount, const void* data);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

public:
    u32 m_vertexCount;
};