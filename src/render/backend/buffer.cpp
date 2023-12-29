#include "buffer.h"
#include "vk_error.h"

#include <stdexcept>
#include <string.h>

u32 vulkanFindMemoryType(VkPhysicalDevice physicalDevice, u32 typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

Buffer::Buffer(VmaAllocator allocator, size_t size, VkBufferUsageFlags bufferUsage, VmaAllocationCreateFlagBits memoryFlags)
    : m_allocator (allocator)
    , m_size      (size)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = bufferUsage;
    
    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = memoryFlags;

    vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo, &m_buffer, &m_memory, nullptr);

    // VkBufferCreateInfo bufferInfo{};
    // bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // bufferInfo.size = size;
    // bufferInfo.usage = usage;
    // bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // vk(vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer));

    // VkMemoryRequirements memRequirements;
    // vkGetBufferMemoryRequirements(device, m_buffer, &memRequirements);

    // VkMemoryAllocateInfo allocInfo{};
    // allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    // allocInfo.allocationSize = memRequirements.size;
    // allocInfo.memoryTypeIndex = vulkanFindMemoryType(physicalDevice, memRequirements.memoryTypeBits, memoryProperties);

    // vk(vkAllocateMemory(device, &allocInfo, nullptr, &m_memory));
    // vk(vkBindBufferMemory(device, m_buffer, m_memory, 0));
}

Buffer::~Buffer() {
    //vkDestroyBuffer(m_device, m_buffer, nullptr);
    //vkFreeMemory(m_device, m_memory, nullptr);
    vmaDestroyBuffer(m_allocator, m_buffer, m_memory);
}

void Buffer::setData(const void* data) {
    void* mappedPtr = map();
    memcpy(mappedPtr, data, m_size);
    unmap();
}

void Buffer::setData(u32 offset, u32 size, const void* data) {
    void* mappedPtr = map(offset, size);
    memcpy(mappedPtr, data, size);
    unmap();
}

void* Buffer::map() {
    void* mappedPtr;
    vk(vmaMapMemory(m_allocator, m_memory, &mappedPtr));
    //vk(vkMapMemory(m_device, m_memory, 0, m_size, 0, &mappedPtr));
    return mappedPtr;
}

void* Buffer::map(u32 offset, u32 size) {
    void* mappedPtr;
    vk(vmaMapMemory(m_allocator, m_memory, &mappedPtr));
    //vk(vkMapMemory(m_device, m_memory, offset, size, 0, &mappedPtr));
    return mappedPtr;
}

void Buffer::unmap() {
    vmaUnmapMemory(m_allocator, m_memory);
    //vkUnmapMemory(m_device, m_memory);
}