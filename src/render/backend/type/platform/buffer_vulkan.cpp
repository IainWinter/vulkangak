#include "render/backend/type/platform/buffer_vulkan.h"
#include "render/backend/type/platform/vk_error.h"

void BufferVulkan::setData(const void* data) {
    void* mappedPtr = map();
    memcpy(mappedPtr, data, bufferSize);
    unmap();
}

void BufferVulkan::setData(u32 offset, u32 size, const void* data) {
    void* mappedPtr = map(offset, size);
    memcpy(mappedPtr, data, size);
    unmap();
}

void* BufferVulkan::map() {
    void* mappedPtr;
    vk(vmaMapMemory(allocator, allocation, &mappedPtr));
    return mappedPtr;
}

void* BufferVulkan::map(u32 offset, u32 size) {
    void* mappedPtr;
    vk(vmaMapMemory(allocator, allocation, &mappedPtr));
    return mappedPtr;
}

void BufferVulkan::unmap() {
    vmaUnmapMemory(allocator, allocation);
}