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
    std::vector<VulkanVertexLayout> vertexInputs;
};

class VertexLayoutBuilder {
public:
    VertexLayoutBuilder& buffer(size_t size, bool instanced);
    VertexLayoutBuilder& attribute(u32 location, size_t offset, VkFormat format);
    std::vector<VulkanVertexLayout> build() const;

private:
    std::vector<VulkanVertexLayout> buffers;
};

class Shader {
public:
    Shader(VkDevice device, VkRenderPass renderPass, const VulkanShaderSource& source);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    VulkanPushConstant getPushConstant(u32 index);

public:
    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout;

private:
    VkDevice m_device;
    std::vector<VulkanPushConstant> m_pushConstants;
};