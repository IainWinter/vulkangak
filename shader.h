#pragma once

#include "typedef.h"
#include "vulkan/vulkan.h"
#include <vector>

struct VulkanVertexLayout {
    VkVertexInputBindingDescription description;
    std::vector<VkVertexInputAttributeDescription> attributes;
};

struct VulkanPushConstant {
    VkShaderStageFlags stages;
    size_t size;
};

struct VulkanShaderSource {
    std::vector<char> vertex;
    std::vector<char> fragment;

    std::vector<VulkanPushConstant> pushConstants;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
};

class Shader {
public:
    Shader(VkDevice device, VkRenderPass renderPass, const VulkanVertexLayout& vertexLayout, const VulkanShaderSource& source);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use(VkCommandBuffer commandBuffer);
    void pushConstant(VkCommandBuffer commandBuffer, size_t i, const void* data);
    void bindDescriptorSet(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet);

    VkPipelineLayout m_pipelineLayout;
private:
    VkDevice m_device;
    VkPipeline m_pipeline;

    std::vector<VulkanPushConstant> m_pushConstants;
};