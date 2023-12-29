#pragma once

#include "typedef.h"
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

u32 vulkanFindMemoryType(VkPhysicalDevice physicalDevice, u32 typeFilter, VkMemoryPropertyFlags properties);

class Buffer {
public:
    Buffer(VmaAllocator allocator, size_t size, VkBufferUsageFlags bufferUsage, VmaAllocationCreateFlagBits flags);
    virtual ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    // memcpy from data to data + size (from constructor) into a mapped region device memory
    void setData(const void* data);
    void setData(u32 offset, u32 size, const void* data);

    void* map();
    void* map(u32 offset, u32 size);

    void unmap();

protected:
    VmaAllocator m_allocator;

public:
    VkBuffer m_buffer;
    VmaAllocation m_memory;

    size_t m_size;
};