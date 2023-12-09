#include <stdexcept>

#define vk(stmt) if (stmt != VK_SUCCESS) { throw std::runtime_error("Vulkan error"); }