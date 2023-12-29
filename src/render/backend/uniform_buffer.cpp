#include "uniform_buffer.h"

#include <string.h>

UniformBuffer::UniformBuffer(VmaAllocator allocator, size_t size) 
    : Buffer (allocator,
              size,
              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
{
    m_persistentMappedPtr = map();
}

void UniformBuffer::setData(const void* data) {
    memcpy(m_persistentMappedPtr, data, m_size);
}

UniformBuffer::~UniformBuffer() {
    unmap(); // may be useless
}