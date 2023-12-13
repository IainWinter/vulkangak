#pragma once

#include "typedef.h"
#include "vulkan/vulkan.h"
#include <vector>

// Forward declarations
class Shader;
class VertexBuffer;
class IndexBuffer;

class CommandBuffer {
public:
    CommandBuffer(VkDevice device, VkCommandPool commandPool);
    ~CommandBuffer();
    
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

    void begin();
    void end();
    void submit(VkQueue queue);
    void reset();

    void copyBuffer(VkBuffer src, VkBuffer dst, size_t size);
    void copyBufferToImage(VkBuffer src, VkImage dst, u32 width, u32 height);

    void transitionImageFormat(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

    void setShader(Shader* shader);
    void pushConstant(Shader* shader, u32 index, const void* data);
    void bindDescriptorSet(Shader* shader, VkDescriptorSet descriptorSet);

    void bindVertexBuffers(u32 count, VertexBuffer* vertexBuffers[]);
    void bindIndexBuffer(IndexBuffer* indexBuffer);

    void draw(u32 vertexCount, u32 instanceCount);
    void drawIndexed(u32 indexCount, u32 instanceCount);

public:
    VkCommandBuffer m_commandBuffer;

private:
    VkDevice m_device;
    VkCommandPool m_commandPool;

    // This is a little hack to stop reallocating vertex buffers a bunch in bindVertexBuffers
    // 4 limit is arbitrary
    VkBuffer m_vertexBuffers[4];
    VkDeviceSize m_offsets[4];
};