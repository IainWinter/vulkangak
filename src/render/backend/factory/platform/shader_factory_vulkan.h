#pragma once

#include "render/backend/factory/shader_factory.h"
#include "render/backend/type/platform/shader_vulkan.h"

class ShaderFactoryVulkan : public ShaderFactory {
public:
    ShaderFactoryVulkan(VkDevice logicalDevice, VkRenderPass renderPass);
    ~ShaderFactoryVulkan();

    Shader* createShader(const ShaderProgramSource& source) override;

    void destroyShader(Shader* shader) override;

private:
    VkDevice m_logicalDevice;
    VkRenderPass m_renderPass;
};