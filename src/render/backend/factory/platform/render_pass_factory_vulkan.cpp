#include "render/backend/factory/platform/render_pass_factory_vulkan.h"
#include "render/backend/type/platform/render_pass_vulkan.h"
#include "render/backend/type/platform/vk_error.h"

RenderPassFactoryVulkan::RenderPassFactoryVulkan(VkDevice logicalDevice)
    : m_logicalDevice (logicalDevice)
{}

RenderPass* RenderPassFactoryVulkan::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB; //m_format.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    
    // This is to fix a sync issue between frames?
    VkSubpassDependency dependencyInfo{};
    dependencyInfo.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencyInfo.dstSubpass = 0;
    dependencyInfo.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencyInfo.srcAccessMask = 0;
    dependencyInfo.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencyInfo.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependencyInfo;

    VkRenderPass vkRenderPass;
    vk(vkCreateRenderPass(m_logicalDevice, &renderPassInfo, nullptr, &vkRenderPass));

    RenderPassVulkan* renderPass = new RenderPassVulkan();
    renderPass->renderPass = vkRenderPass;

    return renderPass;
}

void RenderPassFactoryVulkan::destroyRenderPass(RenderPass* renderPass) {
    RenderPassVulkan* vkRenderPass = static_cast<RenderPassVulkan*>(renderPass);

    vkDestroyRenderPass(m_logicalDevice, vkRenderPass->renderPass, nullptr);
    delete vkRenderPass;
}