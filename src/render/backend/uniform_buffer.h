#pragma once

#include "buffer.h"

class UniformBuffer : public Buffer {
public:
    UniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t size);
    ~UniformBuffer();

    void setData(const void* data);

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;

private:
    void* m_persistentMappedPtr;
};