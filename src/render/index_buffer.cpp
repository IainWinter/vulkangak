#include "index_buffer.h"

IndexBuffer::IndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t indexCount, const u8* data)
    : Buffer       (device, 
                    physicalDevice, 
                    sizeof(u32) * indexCount,
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
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