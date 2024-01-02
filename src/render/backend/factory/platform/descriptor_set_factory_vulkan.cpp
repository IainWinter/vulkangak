#include "render/backend/factory/platform/descriptor_set_factory_vulkan.h"
#include "render/backend/type/platform/descriptor_set_vulkan.h"
#include "render/backend/type/platform/translation_vulkan.h"
#include "render/backend/vk_error.h"
#include <unordered_map>

DescriptorSetFactoryVulkan::DescriptorSetFactoryVulkan(VkDevice logicalDevice, const DescriptorPoolLayout& poolLayout, const FrameSyncInfo& frameSyncInfo)
    : m_logicalDevice  (logicalDevice)
    , m_frameSyncInfo  (frameSyncInfo)
    , m_pool           (VK_NULL_HANDLE)
{
    u32 framesInFlight = frameSyncInfo.framesInFlight();

    std::vector<VkDescriptorPoolSize> poolSizes;
    for (const DescriptorPoolLayout::Pool& pool : poolLayout.pools) {
        VkDescriptorPoolSize vkPool;
        vkPool.type = descriptorTypeMapVulkan(pool.type);
        vkPool.descriptorCount = pool.count * framesInFlight;

        poolSizes.push_back(vkPool);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = (u32)poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = poolLayout.maxSetCount * framesInFlight;

    vk(vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, &m_pool));
}

DescriptorSetFactoryVulkan::~DescriptorSetFactoryVulkan() {
    vkDestroyDescriptorPool(m_logicalDevice, m_pool, nullptr);
}

DescriptorSet* DescriptorSetFactoryVulkan::createDescriptorSet(const DescriptorSetLayout& layout) {
    std::vector<VkDescriptorSetLayoutBinding> vkBindings;
    vkBindings.reserve(layout.bindings.size());

    for (const DescriptorSetLayout::Binding& binding : layout.bindings) {
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

    std::vector<VkDescriptorSet> vkSets;
    vkSets.reserve(m_frameSyncInfo.framesInFlight());

    // Create a set for each layout for each frame in flight
    // could make optional
    for (u32 i = 0; i < m_frameSyncInfo.framesInFlight(); i++) {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &vkLayout;

        VkDescriptorSet vkSet;
        vk(vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, &vkSet));

        vkSets.push_back(vkSet);
    }

    DescriptorSetVulkan* set = new DescriptorSetVulkan();
    set->logicalDevice = m_logicalDevice;
    set->layout = vkLayout;
    set->sets = vkSets;

    return set;
}

void DescriptorSetFactoryVulkan::destroyDescriptorSet(DescriptorSet* descriptorSet) {
    DescriptorSetVulkan* set = (DescriptorSetVulkan*)descriptorSet;

    vkDestroyDescriptorSetLayout(m_logicalDevice, set->layout, nullptr);
    vkFreeDescriptorSets(m_logicalDevice, m_pool, (u32)set->sets.size(), set->sets.data());
    delete set;
}