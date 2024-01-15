#include "render/backend/factory/platform/shader_factory_vulkan.h"
#include "render/backend/type/platform/descriptor_set_layout_vulkan.h"
#include "render/backend/type/platform/render_pass_vulkan.h"
#include "render/backend/type/platform/translation_vulkan.h"
#include "render/backend/type/platform/vk_error.h"

ShaderFactoryVulkan::ShaderFactoryVulkan(VkDevice logicalDevice, RenderPass* defaultRenderPass) 
    : m_logicalDevice     (logicalDevice)
    , m_defaultRenderPass (defaultRenderPass)
{}

ShaderFactoryVulkan::~ShaderFactoryVulkan() {
}

Shader* ShaderFactoryVulkan::createShader(const ShaderProgramSource& source) {
    return createShaderRenderPass(m_defaultRenderPass, source);
}

Shader* ShaderFactoryVulkan::createShaderRenderPass(RenderPass* renderPass, const ShaderProgramSource& source) {
    RenderPassVulkan* vkRenderPass = static_cast<RenderPassVulkan*>(renderPass);

    // Assembly, this defines the type of primitive to draw

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyInfo{};
    pipelineInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
    
    // Vertex input, this defines the vertex format and how to combine multiple vertex buffers together

    // Need to unroll the vertex input layouts
    std::vector<VkVertexInputBindingDescription> vkBindings;
    std::vector<VkVertexInputAttributeDescription> vkAttributes;
    for (const VertexLayout::Buffer& vertexBufferLayout : source.vertexLayout.buffers) {
        VkVertexInputBindingDescription bufferBinding{};
        bufferBinding.binding = vertexBufferLayout.binding;
        bufferBinding.stride = vertexBufferLayout.stride;
        bufferBinding.inputRate = vertexBufferLayout.instanced 
            ? VK_VERTEX_INPUT_RATE_INSTANCE 
            : VK_VERTEX_INPUT_RATE_VERTEX;

        vkBindings.push_back(bufferBinding);

        for (const VertexLayout::Buffer::Attribute& attribute : vertexBufferLayout.attributes) {
            VkVertexInputAttributeDescription vkAttribute{};
            vkAttribute.binding = vertexBufferLayout.binding;
            vkAttribute.location = attribute.location;
            vkAttribute.format = attributeFormatMapVulkan(attribute.format);
            vkAttribute.offset = attribute.offset;

            vkAttributes.push_back(vkAttribute);
        }
    }

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputInfo{};
    pipelineVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputInfo.vertexBindingDescriptionCount = (u32)vkBindings.size();
    pipelineVertexInputInfo.pVertexBindingDescriptions = vkBindings.data();
    pipelineVertexInputInfo.vertexAttributeDescriptionCount = (u32)vkAttributes.size();
    pipelineVertexInputInfo.pVertexAttributeDescriptions = vkAttributes.data();

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

    switch (source.blendType) {
        default:
        case BlendType_None: {
            break;
        } 
        case BlendType_Additive: {
            pipelineColorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            pipelineColorBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
            pipelineColorBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            pipelineColorBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            pipelineColorBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;
            pipelineColorBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;
            break;
        }
        case BlendType_Alpha: {
            pipelineColorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            pipelineColorBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            pipelineColorBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            pipelineColorBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            pipelineColorBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;
            pipelineColorBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;
            break;
        }
    }

    VkPipelineColorBlendStateCreateInfo pipelineColorBlending{};
    pipelineColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipelineColorBlending.logicOpEnable = VK_FALSE;
    pipelineColorBlending.attachmentCount = 1;
    pipelineColorBlending.pAttachments = &pipelineColorBlendAttachmentInfo;

    std::vector<VkPushConstantRange> vkPushConstants;
    vkPushConstants.reserve(source.pushConstants.size());

    for (const ShaderProgramSource::PushConstant& pushConstant : source.pushConstants) {
        VkPushConstantRange range{};
        range.stageFlags = shaderStageFlagMapVulkan(pushConstant.stageBits);
        range.size = pushConstant.size;
        range.offset = 0;

        vkPushConstants.push_back(range);
    }

    std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts;
    vkDescriptorSetLayouts.reserve(source.descriptorSetLayouts.size());

    for (const DescriptorSetLayout* layout : source.descriptorSetLayouts) {
        DescriptorSetLayoutVulkan* vkLayout = (DescriptorSetLayoutVulkan*)layout;
        vkDescriptorSetLayouts.push_back(vkLayout->layout);
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = (u32)vkPushConstants.size();
    pipelineLayoutInfo.pPushConstantRanges = vkPushConstants.data();
    pipelineLayoutInfo.setLayoutCount = (u32)vkDescriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = vkDescriptorSetLayouts.data();

    VkPipelineLayout vkPipelineLayout;
    vk(vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &vkPipelineLayout));

    //  Create the pipeline

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    shaderStages.reserve(source.shaders.size());

    for (const ShaderProgramSource::ShaderSource& shaderSource : source.shaders) {
        VkShaderModuleCreateInfo shaderInfo{};
        shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderInfo.codeSize = shaderSource.source.size();
        shaderInfo.pCode = (u32*)shaderSource.source.data();

        VkShaderModule vkModule;
        vk(vkCreateShaderModule(m_logicalDevice, &shaderInfo, nullptr, &vkModule));    

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = shaderStageFlagMapVulkan(shaderSource.stageBit);
        vertShaderStageInfo.module = vkModule;
        vertShaderStageInfo.pName = "main";

        shaderStages.push_back(vertShaderStageInfo);
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = (u32)shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &pipelineVertexInputInfo;
    pipelineInfo.pInputAssemblyState = &pipelineInputAssemblyInfo;
    pipelineInfo.pViewportState = &pipelineViewportStateInfo;
    pipelineInfo.pRasterizationState = &pipelineRasterizerInfo;
    pipelineInfo.pMultisampleState = &pipelineMultisampleInfo;
    pipelineInfo.pColorBlendState = &pipelineColorBlending;
    pipelineInfo.pDynamicState = &pipelineDynamicStateInfo;
    pipelineInfo.layout = vkPipelineLayout;
    pipelineInfo.renderPass = vkRenderPass->renderPass;
    pipelineInfo.subpass = 0;

    VkPipeline vkPipeline;
    vk(vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkPipeline));

    for (const VkPipelineShaderStageCreateInfo& shaderStage : shaderStages) {
        vkDestroyShaderModule(m_logicalDevice, shaderStage.module, nullptr);
    }

    ShaderVulkan* shader = new ShaderVulkan();
    shader->pipeline = vkPipeline;
    shader->pipelineLayout = vkPipelineLayout;
    shader->pushConstants = vkPushConstants;

    return shader;
}

void ShaderFactoryVulkan::destroyShader(Shader* shader) {
    ShaderVulkan* shaderVulkan = (ShaderVulkan*)shader;

    vkDestroyPipeline(m_logicalDevice, shaderVulkan->pipeline, nullptr);
    vkDestroyPipelineLayout(m_logicalDevice, shaderVulkan->pipelineLayout, nullptr);
    delete shader;
}