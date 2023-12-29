#include "image.h"
#include "command_buffer.h"
#include "vk_error.h"

Image::Image(VmaAllocator allocator, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkFormat format, const u8* pixels, u32 width, u32 height)
    : m_device (device)
{
    // Only handle RGBA
    size_t size = width * height * 4;

    Buffer stagingBuffer = Buffer(allocator, size, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
    );
    
    stagingBuffer.setData(pixels);

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    
    vk(vkCreateImage(device, &imageInfo, nullptr, &m_image));

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, m_image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vulkanFindMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vk(vkAllocateMemory(device, &allocInfo, nullptr, &m_imageMemory));
    vk(vkBindImageMemory(device, m_image, m_imageMemory, 0));

    {
        CommandBuffer cmdBuffer = CommandBuffer(device, commandPool);
        cmdBuffer.begin();
        cmdBuffer.transitionImageFormat(m_image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        cmdBuffer.end();
        cmdBuffer.submit(graphicsQueue);
    }

    {
        CommandBuffer cmdBuffer = CommandBuffer(device, commandPool);
        cmdBuffer.begin();
        cmdBuffer.copyBufferToImage(stagingBuffer.m_buffer, m_image, width, height);
        cmdBuffer.end();
        cmdBuffer.submit(graphicsQueue);
    }

    {
        CommandBuffer cmdBuffer = CommandBuffer(device, commandPool);
        cmdBuffer.begin();
        cmdBuffer.transitionImageFormat(m_image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        cmdBuffer.end();
        cmdBuffer.submit(graphicsQueue);
    }    

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vk(vkCreateImageView(device, &viewInfo, nullptr, &m_view));
}

Image::~Image() {
    vkDestroyImageView(m_device, m_view, nullptr);
    vkDestroyImage(m_device, m_image, nullptr);
    vkFreeMemory(m_device, m_imageMemory, nullptr);
}

VkImageView Image::getView() const {
    return m_view;
}

ImageSampler::ImageSampler(VkDevice device) 
    : m_device (device)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16; // should pass this in, from deviceprops.limits.maxSamplerAnisotropy
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    vk(vkCreateSampler(device, &samplerInfo, nullptr, &m_sampler));
}

ImageSampler::~ImageSampler() {
    vkDestroySampler(m_device, m_sampler, nullptr);
}