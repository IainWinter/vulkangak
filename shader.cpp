#include "shader.h"
#include "vk_error.h"
#include <stdexcept>

Shader::Shader(VkDevice device, VkRenderPass renderPass, const VulkanVertexLayout& vertexLayout, const VulkanShaderSource& source) 
    : m_device        (device)
    , m_pushConstants (source.pushConstants)
{
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyInfo{};
    pipelineInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputInfo{};
    pipelineVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputInfo.vertexBindingDescriptionCount = 1;
    pipelineVertexInputInfo.pVertexBindingDescriptions = &vertexLayout.description;
    pipelineVertexInputInfo.vertexAttributeDescriptionCount = vertexLayout.attributes.size();
    pipelineVertexInputInfo.pVertexAttributeDescriptions = vertexLayout.attributes.data();

    std::vector<VkDynamicState> pipelineDynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateInfo{};
    pipelineDynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineDynamicStateInfo.dynamicStateCount = (u32)pipelineDynamicStates.size();
    pipelineDynamicStateInfo.pDynamicStates = pipelineDynamicStates.data();

    VkPipelineViewportStateCreateInfo pipelineViewportStateInfo{};
    pipelineViewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineViewportStateInfo.viewportCount = 1;
    pipelineViewportStateInfo.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo pipelineRasterizerInfo{};
    pipelineRasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipelineRasterizerInfo.depthClampEnable = VK_FALSE;
    pipelineRasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
    pipelineRasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
    pipelineRasterizerInfo.lineWidth = 1.0f;
    pipelineRasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    pipelineRasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    pipelineRasterizerInfo.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleInfo{};
    pipelineMultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineMultisampleInfo.sampleShadingEnable = VK_FALSE;
    pipelineMultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentInfo{};
    pipelineColorBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pipelineColorBlendAttachmentInfo.blendEnable = VK_TRUE;
    pipelineColorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    pipelineColorBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    pipelineColorBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;
    pipelineColorBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    pipelineColorBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipelineColorBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo pipelineColorBlending{};
    pipelineColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipelineColorBlending.logicOpEnable = VK_FALSE;
    pipelineColorBlending.attachmentCount = 1;
    pipelineColorBlending.pAttachments = &pipelineColorBlendAttachmentInfo;

    std::vector<VkPushConstantRange> pushConstants;
    pushConstants.reserve(source.pushConstants.size());

    for (const VulkanPushConstant& pushConstant : source.pushConstants) {
        VkPushConstantRange range{};
        range.stageFlags = pushConstant.stages;
        range.size = pushConstant.size;
        range.offset = 0; 

        pushConstants.push_back(range);
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = (u32)pushConstants.size();
    pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();
    pipelineLayoutInfo.setLayoutCount = (u32)source.descriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = source.descriptorSetLayouts.data();

    vk(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout));

    //  Create the pipeline

    VkShaderModuleCreateInfo shaderInfo{};
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = source.vertex.size();
    shaderInfo.pCode = (u32*)source.vertex.data();

    VkShaderModule vertexModule;
    vk(vkCreateShaderModule(device, &shaderInfo, nullptr, &vertexModule));

    // should rename
    shaderInfo = {};
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = source.fragment.size();
    shaderInfo.pCode = (u32*)source.fragment.data();

    VkShaderModule fragmentModule;
    vk(vkCreateShaderModule(device, &shaderInfo, nullptr, &fragmentModule));

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertexModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragmentModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &pipelineVertexInputInfo;
    pipelineInfo.pInputAssemblyState = &pipelineInputAssemblyInfo;
    pipelineInfo.pViewportState = &pipelineViewportStateInfo;
    pipelineInfo.pRasterizationState = &pipelineRasterizerInfo;
    pipelineInfo.pMultisampleState = &pipelineMultisampleInfo;
    pipelineInfo.pColorBlendState = &pipelineColorBlending;
    pipelineInfo.pDynamicState = &pipelineDynamicStateInfo;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    vk(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));

    vkDestroyShaderModule(device, vertexModule, nullptr);
    vkDestroyShaderModule(device, fragmentModule, nullptr);
}

Shader::~Shader() {
    vkDestroyPipeline(m_device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
}

void Shader::use(VkCommandBuffer commandBuffer) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
}

void Shader::pushConstant(VkCommandBuffer commandBuffer, size_t i, const void* data) {
    if (i >= m_pushConstants.size()) {
        throw std::runtime_error("Mismatched number of push constants");
    }

    const VulkanPushConstant& pushConstant = m_pushConstants.at(i);
    vkCmdPushConstants(commandBuffer, m_pipelineLayout, pushConstant.stages, 0, pushConstant.size, data);
}

void Shader::bindDescriptorSet(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet) {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);    
}