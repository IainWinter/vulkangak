#include "index_buffer.h"

IndexBuffer::IndexBuffer(VmaAllocator allocator, size_t indexCount, const u8* data)
    : Buffer       (allocator,
                    sizeof(u32) * indexCount,
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
    , m_indexCount (indexCount)
{
    setData(data);
}

IndexBuffer::~IndexBuffer() {
}

void IndexBuffer::bind(VkCommandBuffer commandBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, m_buffer, 0, VK_INDEX_TYPE_UINT32);
}

void IndexBuffer::draw(VkCommandBuffer commandBuffer) {
    vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
}