#include "render/backend/factory/platform/frame_buffer_factory_vulkan.h"
#include "render/backend/type/platform/frame_buffer_vulkan.h"
#include "render/backend/type/platform/render_pass_vulkan.h"
#include "render/backend/type/platform/image_vulkan.h"
#include "render/backend/type/platform/vk_error.h"

FrameBufferFactoryVulkan::FrameBufferFactoryVulkan(VkDevice logicalDevice)
    : m_logicalDevice (logicalDevice)
{}

FrameBuffer* FrameBufferFactoryVulkan::createFrameBufferSimple(RenderPass* renderPass, Image* attachment, u32 width, u32 height) {
    RenderPassVulkan* vkRenderPass = static_cast<RenderPassVulkan*>(renderPass);
    ImageVulkan* vkImage = static_cast<ImageVulkan*>(attachment);

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vkRenderPass->renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = &vkImage->view;
    framebufferInfo.width =  width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    VkFramebuffer vkFramebuffer;
    vk(vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &vkFramebuffer));

    FrameBufferVulkan* frameBuffer = new FrameBufferVulkan();
    frameBuffer->frameBuffer = vkFramebuffer;

    return frameBuffer;
}

void FrameBufferFactoryVulkan::destroyFrameBuffer(FrameBuffer* frameBuffer) {
    FrameBufferVulkan* vkFrameBuffer = static_cast<FrameBufferVulkan*>(frameBuffer);

    vkDestroyFramebuffer(m_logicalDevice, vkFrameBuffer->frameBuffer, nullptr);
    delete vkFrameBuffer;
}