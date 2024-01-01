#pragma once

#include "render/backend/factory/buffer_factory.h"
#include "render/backend/type/platform/buffer_vulkan.h"

class BufferFactoryVulkan : public BufferFactory {
public:
    BufferFactoryVulkan(VkDevice logicalDevice, VmaAllocator allocator);
    ~BufferFactoryVulkan();

    Buffer* createBufferEmpty(BufferType type, size_t size) override;
    Buffer* createBuffer(BufferType type, size_t size, const u8* data) override;

    Buffer* createVertexBufferEmpty(size_t vertexSize, size_t vertexCount) override;
    Buffer* createVertexBuffer(size_t vertexSize, size_t vertexCount, const u8* vertexData) override;

    Buffer* createIndexBufferEmpty(size_t indexCount) override;
    Buffer* createIndexBuffer(size_t indexCount, const u32* indexData) override;

    Buffer* createUniformBufferEmpty(size_t size) override;

    void destroyBuffer(Buffer* buffer) override;

private:
    VkDevice m_logicalDevice;
    VmaAllocator m_allocator;
};