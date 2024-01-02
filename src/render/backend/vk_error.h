#include <stdexcept>
#include <stdio.h>

inline void vk(VkResult result) {
    if (result != VK_SUCCESS) {
        printf("Failed ");
        throw std::runtime_error("Vulkan error");
    }
}

inline void err(bool result, const char* message = "An error has occured") {
    if (!result) {
        printf("Error: %s", message);
        throw std::runtime_error(message);
    }
}