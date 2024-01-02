#pragma once

#include "render/backend/factory/platform/buffer_factory_vulkan.h"
#include "render/backend/type/platform/translation_vulkan.h"
#include "render/backend/vk_error.h"

BufferFactoryVulkan::BufferFactoryVulkan(VkDevice logicalDevice, VmaAllocator allocator) 
    : m_logicalDevice (logicalDevice)
    , m_allocator     (allocator)
{}

BufferFactoryVulkan::~BufferFactoryVulkan() {
    
}

Buffer* BufferFactoryVulkan::createBufferEmpty(BufferType type, size_t size) {
    VkBufferUsageFlagBits vkBufferType = bufferTypeMapVulkan(type);
    VmaAllocationCreateFlags vkMemoryType = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = vkBufferType;
    
    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = vkMemoryType;

    VkBuffer vkBuffer;
    VmaAllocation vkAllocation;
    vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo, &vkBuffer, &vkAllocation, nullptr);

    BufferVulkan* buffer = new BufferVulkan();
    buffer->buffer = vkBuffer;
    buffer->bufferSize = size;
    buffer->allocation = vkAllocation;
    buffer->allocator = m_allocator;

    return buffer;
}

Buffer* BufferFactoryVulkan::createBuffer(BufferType type, size_t size, const u8* data) {
    Buffer* buffer = createBufferEmpty(type, size);
    buffer->setData(data);
    return buffer;
}

Buffer* BufferFactoryVulkan::createVertexBufferEmpty(size_t vertexSize, size_t vertexCount) {
    return createBufferEmpty(BufferType_Vertex, vertexSize * vertexCount);
}

Buffer* BufferFactoryVulkan::createVertexBuffer(size_t vertexSize, size_t vertexCount, const u8* vertexData) {
    return createBuffer(BufferType_Vertex, vertexSize * vertexCount, vertexData);
}

Buffer* BufferFactoryVulkan::createIndexBufferEmpty(size_t indexCount) {
    return createBufferEmpty(BufferType_Index, sizeof(u32) * indexCount);
}

Buffer* BufferFactoryVulkan::createIndexBuffer(size_t indexCount, const u32* indexData) {
    return createBuffer(BufferType_Index, sizeof(u32) * indexCount, (const u8*)indexData);
}

Buffer* BufferFactoryVulkan::createUniformBufferEmpty(size_t size) {
    return createBufferEmpty(BufferType_Uniform, size);
}

void BufferFactoryVulkan::destroyBuffer(Buffer* buffer) {
    BufferVulkan* bufferVulkan = static_cast<BufferVulkan*>(buffer);
    
    vmaDestroyBuffer(m_allocator, bufferVulkan->buffer, bufferVulkan->allocation);
    delete bufferVulkan;
}