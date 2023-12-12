#include "uniform_buffer.h"

UniformBuffer::UniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t size) 
    : Buffer (device, 
              physicalDevice, 
              size,
              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
{
    m_persistentMappedPtr = map();
}

void UniformBuffer::setData(const void* data) {
    memcpy(m_persistentMappedPtr, data, m_size);
}

UniformBuffer::~UniformBuffer() {
    unmap(); // may be useless
}