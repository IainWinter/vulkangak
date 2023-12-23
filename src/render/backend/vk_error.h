#include <stdexcept>
#include <stdio.h>

//#define vk(stmt) if (stmt != VK_SUCCESS) { throw std::runtime_error("Vulkan error"); }

inline void vk(VkResult result) {
    if (result != VK_SUCCESS) {
        printf("Failed ");
        throw std::runtime_error("Vulkan error");
    }
}