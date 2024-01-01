#pragma once

#include "render/backend/factory/platform/image_factory_vulkan.h"
#include "render/backend/type/platform/buffer_vulkan.h"
#include "render/backend/vk_error.h"

// remove this
#include "render/backend/type/buffer.h"
#include "render/backend/command_buffer.h"


struct VulkanImageFormat {
    size_t pixelSize;
    size_t elementSize;
    VkFormat format;
};

static VulkanImageFormat s_imageFormatMap[] = {
    { 8, 24, VK_FORMAT_R8G8B8_SRGB },
    { 8, 24, VK_FORMAT_R8G8B8_UNORM },
    { 8, 32, VK_FORMAT_R8G8B8A8_SRGB },
    { 8, 32, VK_FORMAT_R8G8B8A8_UNORM }
};

static VkSampleCountFlagBits s_imageSampleCountMap[] = {
    VK_SAMPLE_COUNT_1_BIT,
    VK_SAMPLE_COUNT_2_BIT,
    VK_SAMPLE_COUNT_4_BIT,
    VK_SAMPLE_COUNT_8_BIT,
    VK_SAMPLE_COUNT_16_BIT,
    VK_SAMPLE_COUNT_32_BIT,
    VK_SAMPLE_COUNT_64_BIT
};

ImageFactoryVulkan::ImageFactoryVulkan(VkDevice logicalDevice, VmaAllocator allocator, VkCommandPool commandPool, VkQueue graphicsQueue, BufferFactory* bufferFactory) 
    : m_allocator     (allocator)
    , m_logicalDevice (logicalDevice)
    , m_commandPool   (commandPool)
    , m_graphicsQueue (graphicsQueue)
    , m_bufferFactory (bufferFactory)
{}

ImageFactoryVulkan::~ImageFactoryVulkan() {
}

Image* ImageFactoryVulkan::createImage2DEmpty(u32 width, u32 height, ImageFormat format) {
    return createImageVulkan2D(width, height, format, ImageSampleCount_1);
}

Image* ImageFactoryVulkan::createImage2DEmptyMultisample(u32 width, u32 height, ImageFormat format, ImageSampleCount sampleCount) {
    return createImageVulkan2D(width, height, format, sampleCount);
}

Image* ImageFactoryVulkan::createImage2D(u32 width, u32 height, ImageFormat format, const u8* pixels) {
    ImageVulkan* image = createImageVulkan2D(width, height, format, ImageSampleCount_1);

    BufferVulkan* transferBuffer = (BufferVulkan*)m_bufferFactory->createBuffer(BufferType_TransferSource, image->imageBufferSize, pixels);

    {
        CommandBuffer cmdBuffer = CommandBuffer(m_logicalDevice, m_commandPool);
        cmdBuffer.begin();
        cmdBuffer.transitionImageFormat(image->image, image->format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        cmdBuffer.end();
        cmdBuffer.submit(m_graphicsQueue);
    }

    {
        CommandBuffer cmdBuffer = CommandBuffer(m_logicalDevice, m_commandPool);
        cmdBuffer.begin();
        cmdBuffer.copyBufferToImage(transferBuffer->buffer, image->image, width, height);
        cmdBuffer.end();
        cmdBuffer.submit(m_graphicsQueue);
    }

    {
        CommandBuffer cmdBuffer = CommandBuffer(m_logicalDevice, m_commandPool);
        cmdBuffer.begin();
        cmdBuffer.transitionImageFormat(image->image, image->format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        cmdBuffer.end();
        cmdBuffer.submit(m_graphicsQueue);
    }

    m_bufferFactory->destroyBuffer(transferBuffer);

    return image;
}

Image* ImageFactoryVulkan::createImage2DFromAsset(const ImageAsset& asset) {
    // can only load 4 channel images
    // some vulkan validation layer thing should investigate why

    bool reformat = asset.channels != 4;
    const u8* pixelsFormatted = (const u8*)asset.pixels.data();

    if (reformat) {
        u8* p = new u8[asset.width * asset.height * 4];

        for (u32 i = 0; i < asset.width * asset.height; i++) {
            p[i * 4 + 0] = pixelsFormatted[i * 3 + 0];
            p[i * 4 + 1] = pixelsFormatted[i * 3 + 1];
            p[i * 4 + 2] = pixelsFormatted[i * 3 + 2];
            p[i * 4 + 3] = 0xFF;
        }

        pixelsFormatted = p;
    }

    Image* image = createImage2D(asset.width, asset.height, ImageFormat_RGBA_8_UNORM, pixelsFormatted);

    // ew
    if (reformat) {
        delete[] pixelsFormatted;
    }

    return image;
}

void ImageFactoryVulkan::destroyImage(Image* image) {
    ImageVulkan* imageVulkan = (ImageVulkan*)image;

    vkDestroyImageView(m_logicalDevice, imageVulkan->view, nullptr);
    vmaDestroyImage(m_allocator, imageVulkan->image, imageVulkan->allocation);
    delete imageVulkan;
}

ImageVulkan* ImageFactoryVulkan::createImageVulkan2D(u32 width, u32 height, ImageFormat format, ImageSampleCount sampleCount) {
    VulkanImageFormat vkFormat = s_imageFormatMap[(int)format];
    VkSampleCountFlagBits vkSampleCount = s_imageSampleCountMap[sampleCount];

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = vkFormat.format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = vkSampleCount;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

    VmaAllocation vkAllocation;
    VkImage vkImage;
    VkImageView vkView;

    vk(vmaCreateImage(m_allocator, &imageInfo, &allocInfo, &vkImage, &vkAllocation, nullptr));

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = vkImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = vkFormat.format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vk(vkCreateImageView(m_logicalDevice, &viewInfo, nullptr, &vkView));

    ImageVulkan* image = new ImageVulkan();
    image->allocation = vkAllocation;
    image->image = vkImage;
    image->view = vkView;
    image->format = vkFormat.format;
    image->pixelSize = vkFormat.pixelSize;
    image->elementSize = vkFormat.elementSize;
    image->width = width;
    image->height = height;
    image->depth = 1;
    image->imageBufferSize = width * height * vkFormat.pixelSize;

    return image;
}