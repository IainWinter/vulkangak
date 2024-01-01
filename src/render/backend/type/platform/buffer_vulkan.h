#pragma once

#include "render/backend/type/buffer.h"

#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

class BufferVulkan : public Buffer {
public:
    VmaAllocation allocation;
    VkBuffer buffer;

    u32 bufferSize;

    // needed for mapping
    VmaAllocator allocator;
    
    void setData(const void* data) override;
    void setData(u32 offset, u32 size, const void* data) override;

    void* map() override;
    void* map(u32 offset, u32 size) override;

    void unmap() override;
};