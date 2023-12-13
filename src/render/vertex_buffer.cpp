#include "vertex_buffer.h"

VertexBuffer::VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t vertexSize, size_t vertexCount, const void* data)
    : Buffer        (device, 
                     physicalDevice, 
                     vertexSize * vertexCount,
                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    , m_vertexCount (vertexCount)
{
    if (data) {
        setData(data);
    }
}

VertexBuffer::~VertexBuffer() {
}