#pragma once

#include "typedef.h"
#include "vulkan/vulkan.h"

u32 vulkanFindMemoryType(VkPhysicalDevice physicalDevice, u32 typeFilter, VkMemoryPropertyFlags properties);

class Buffer {
public:
    Buffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);
    virtual ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    // memcpy from data to data + size (from constructor) into a mapped region device memory
    void setData(const void* data);

    void* map();
    void unmap();

public:
    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
    size_t m_size;

protected:
    VkDevice m_device;
};