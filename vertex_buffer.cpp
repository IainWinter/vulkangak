#include "vertex_buffer.h"

VertexBuffer::VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t vertexSize, size_t vertexCount, const void* data)
    : Buffer        (device, 
                     physicalDevice, 
                     vertexSize * vertexCount,
                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    , m_vertexCount (vertexCount)
{
    setData(data);
}

VertexBuffer::~VertexBuffer() {
}

void VertexBuffer::bind(VkCommandBuffer commandBuffer) {
    VkBuffer vertexBuffers[] = { m_buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}

void VertexBuffer::draw(VkCommandBuffer commandBuffer) {
    vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
}