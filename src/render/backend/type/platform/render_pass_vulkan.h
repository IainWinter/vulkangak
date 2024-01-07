#pragma once

#include "render/backend/type/render_pass.h"
#include ""

class RenderPassVulkan : public RenderPass {
public:
    VkRenderPass renderpass;
};