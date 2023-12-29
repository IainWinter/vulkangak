#include "vertex_buffer.h"

VertexBuffer::VertexBuffer(VmaAllocator allocator, size_t vertexSize, size_t vertexCount, const void* data)
    : Buffer        (allocator,
                     vertexSize * vertexCount,
                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                     VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
    , m_vertexCount (vertexCount)
{
    if (data) {
        setData(data);
    }
}

VertexBuffer::~VertexBuffer() {
}