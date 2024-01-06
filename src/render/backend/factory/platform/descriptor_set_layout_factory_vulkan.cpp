#include "render/backend/factory/platform/descriptor_set_layout_factory_vulkan.h"
#include "render/backend/type/platform/descriptor_set_layout_vulkan.h"
#include "render/backend/type/platform/translation_vulkan.h"
#include "render/backend/type/platform/vk_error.h"
#include <unordered_map>

DescriptorSetLayoutFactoryVulkan::DescriptorSetLayoutFactoryVulkan(VkDevice logicalDevice)
    : m_logicalDevice (logicalDevice)
{}

DescriptorSetLayoutFactoryVulkan::~DescriptorSetLayoutFactoryVulkan() {
}

DescriptorSetLayout* DescriptorSetLayoutFactoryVulkan::createDescriptorSetLayout(const DescriptorSetLayoutConfig& config) {
    std::vector<VkDescriptorSetLayoutBinding> vkBindings;
    vkBindings.reserve(config.bindings.size());

    for (const DescriptorSetLayoutConfig::Binding& binding : config.bindings) {
        VkDescriptorSetLayoutBinding vkBinding{};
        vkBinding.descriptorType = descriptorTypeMapVulkan(binding.type);
        vkBinding.stageFlags = shaderStageFlagMapVulkan(binding.stageBits);
        vkBinding.binding = binding.location;
        vkBinding.descriptorCount = binding.elementCount;

        vkBindings.push_back(vkBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = (u32)vkBindings.size();
    layoutInfo.pBindings = vkBindings.data();

    VkDescriptorSetLayout vkLayout;
    vk(vkCreateDescriptorSetLayout(m_logicalDevice, &layoutInfo, nullptr, &vkLayout));

    DescriptorSetLayoutVulkan* layout = new DescriptorSetLayoutVulkan();
    layout->layout = vkLayout;

    return layout;
}

void DescriptorSetLayoutFactoryVulkan::destroyDescriptorSetLayout(DescriptorSetLayout* layout) {
    DescriptorSetLayoutVulkan* vkLayout = dynamic_cast<DescriptorSetLayoutVulkan*>(layout);

    vkDestroyDescriptorSetLayout(m_logicalDevice, vkLayout->layout, nullptr);
    delete layout;
}