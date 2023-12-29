#pragma once

#include "typedef.h"
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

#include "buffer.h"

class Image {
public:
    // should convert to using the allocator
    Image(VmaAllocator allocator, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue m_graphicsQueue, VkFormat format, const u8* pixels, u32 width, u32 height);
    ~Image();

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    
    VkImageView getView() const;

private:
    VkDevice m_device;
    
    VkImage m_image;
    VkDeviceMemory m_imageMemory;

    // For now just make this with the image, could make a list if there needs to be more than one
    // per texture, ie for mips
    VkImageView m_view;
};

// These have no relation to any instance of an Image
class ImageSampler {
public:
    ImageSampler(VkDevice device);
    ~ImageSampler();

public:
    VkSampler m_sampler;

private:
    VkDevice m_device;
};