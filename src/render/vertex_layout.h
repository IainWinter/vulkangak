#pragma once

#include "typedef.h"
#include "vulkan/vulkan.h"
#include <vector>

struct VulkanVertexLayout {
    VkVertexInputBindingDescription description;
    std::vector<VkVertexInputAttributeDescription> attributes;
};

class VertexLayoutBuilder {
public:
    inline VertexLayoutBuilder& buffer(size_t stride, bool instanced) {
        VulkanVertexLayout buffer{};
        buffer.description.binding = (u32)buffers.size();
        buffer.description.stride = stride;
        buffer.description.inputRate = instanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;

        buffers.push_back(buffer);

        return *this;
    }

    inline VertexLayoutBuilder& attribute(u32 location, size_t offset, VkFormat format) {
        VkVertexInputAttributeDescription attribute{};
        attribute.binding = (u32)buffers.size() - 1;
        attribute.location = location;
        attribute.format = format;
        attribute.offset = offset;

        buffers.back().attributes.push_back(attribute);
        
        return *this;
    }

    inline std::vector<VulkanVertexLayout> build() const {
        return buffers;
    }

private:
    std::vector<VulkanVertexLayout> buffers;
};