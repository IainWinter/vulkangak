#pragma once

#include "vulkan/vulkan.h"

class RenderPass {
public:
    RenderPass(VkDevice logicalDevice, VkFormat color1Format); // todo: add more args
    ~RenderPass();

public:
    VkRenderPass m_vkRenderPass;

private:
    VkDevice m_logicalDevice;
};