#pragma once

#include "render/backend/factory/image_factory.h"
#include "render/backend/factory/buffer_factory.h"
#include "render/backend/factory/command_buffer_factory.h"
#include "render/backend/type/platform/image_vulkan.h"

class ImageFactoryVulkan : public ImageFactory {
public:
    ImageFactoryVulkan(VkDevice logicalDevice, VmaAllocator allocator, VkCommandPool commandPool, VkQueue graphicsQueue, BufferFactory* bufferFactory, CommandBufferFactory* commandBufferFactory);
    ~ImageFactoryVulkan();

    Image* createImage2DEmpty(u32 width, u32 height, ImageFormat format) override;
    Image* createImage2DEmptyMultisample(u32 width, u32 height, ImageFormat format, ImageSampleCount sampleCount) override;

    Image* createImage2D(u32 width, u32 height, ImageFormat format, const u8* pixels) override;

    Image* createImage2DFromAsset(const ImageAsset& asset) override;

    void destroyImage(Image* image) override;

private:
    ImageVulkan* createImageVulkan2D(u32 width, u32 height, ImageFormat format, ImageSampleCount sampleCount);

private:
    VkDevice m_logicalDevice;
    VmaAllocator m_allocator;
    VkCommandPool m_commandPool;
    VkQueue m_graphicsQueue;
    BufferFactory* m_bufferFactory;
    CommandBufferFactory* m_commandBufferFactory;
};