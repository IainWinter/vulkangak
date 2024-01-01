#pragma once

#include "render/backend/type/image.h"

#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

class ImageVulkan : public Image {
public:
    VmaAllocation allocation;
    VkImage image;

    VkImageView view;

    VkFormat format;
    u32 pixelSize;
    u32 elementSize;

    u32 width;
    u32 height;
    u32 depth;
    
    u32 imageBufferSize;
};