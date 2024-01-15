#pragma once

#include "render/backend/factory/shader_factory.h"
#include "render/backend/type/platform/shader_vulkan.h"

class ShaderFactoryVulkan : public ShaderFactory {
public:
    ShaderFactoryVulkan(VkDevice logicalDevice, RenderPass* defaultRenderPass);
    ~ShaderFactoryVulkan();

    Shader* createShader(const ShaderProgramSource& source) override;

    Shader* createShaderRenderPass(RenderPass* renderPass, const ShaderProgramSource& source);

    void destroyShader(Shader* shader) override;

private:
    VkDevice m_logicalDevice;
    RenderPass* m_defaultRenderPass;
};