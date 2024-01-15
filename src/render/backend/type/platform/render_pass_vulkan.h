#pragma once

#include "render/backend/type/render_pass.h"
#include "vulkan/vulkan.h"

class RenderPassVulkan : public RenderPass {
public:
    VkRenderPass renderPass;
};