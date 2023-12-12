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

    void draw(const std::vector<VertexBuffer*>& vertexBuffers);
    void draw(IndexBuffer* indexBuffer, const std::vector<VertexBuffer*>& vertexBuffers);

    void drawInstanced(int instanceCount, const std::vector<VertexBuffer*>& vertexBuffers);
    void drawInstanced(int instanceCount, IndexBuffer* indexBuffer, const std::vector<VertexBuffer*>& vertexBuffers);

public:
    VkCommandBuffer m_commandBuffer;

private:
    VkDevice m_device;
    VkCommandPool m_commandPool;
};