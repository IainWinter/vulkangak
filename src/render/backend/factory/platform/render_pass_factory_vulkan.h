#pragma once

#include "render/backend/factory/render_pass_factory.h"
#include "vulkan/vulkan.h"

class RenderPassFactoryVulkan : public RenderPassFactory {
public:
    RenderPassFactoryVulkan(VkDevice logicalDevice);

    RenderPass* createRenderPass() override;

    void destroyRenderPass(RenderPass* renderPass) override;

private:
    VkDevice m_logicalDevice;
};