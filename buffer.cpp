#include "buffer.h"
#include "vk_error.h"

#include <stdexcept>

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

Buffer::Buffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties)
    : m_device (device)
    , m_size   (size)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vk(vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, m_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vulkanFindMemoryType(physicalDevice, memRequirements.memoryTypeBits, memoryProperties);

    vk(vkAllocateMemory(device, &allocInfo, nullptr, &m_memory));
    vk(vkBindBufferMemory(device, m_buffer, m_memory, 0));
}

Buffer::~Buffer() {
    vkDestroyBuffer(m_device, m_buffer, nullptr);
    vkFreeMemory(m_device, m_memory, nullptr);
}

void Buffer::setData(const void* data) {
    void* mappedPtr = map();
    memcpy(mappedPtr, data, m_size);
    unmap();
}

void* Buffer::map() {
    void* mappedPtr;
    vk(vkMapMemory(m_device, m_memory, 0, m_size, 0, &mappedPtr));
    return mappedPtr;
}

void Buffer::unmap() {
    vkUnmapMemory(m_device, m_memory);
}