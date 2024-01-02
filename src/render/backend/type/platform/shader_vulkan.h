#pragma once

#include "render/backend/type/shader.h"
#include <vector>

#include "vulkan/vulkan.h"

class ShaderVulkan : public Shader {
public:
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    std::vector<VkPushConstantRange> pushConstants;
};