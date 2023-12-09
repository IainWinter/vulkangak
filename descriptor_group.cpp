#include "descriptor_group.h"
#include "vk_error.h"
#include <unordered_map>

struct Descriptor {
    VkDescriptorSetLayout layout;
    VkDescriptorSet set;
};

struct DescriptorSetLayout {
    std::vector<DescriptorBinding> descriptors;
    u32 totalArrayElementCount;
};

DescriptorGroup::DescriptorGroup(VkDevice device, FrameSyncInfo frame, const std::vector<DescriptorBinding>& bindings) 
    : m_device   (device)
    , m_frame    (frame)
    , m_bindings (bindings)
{
    // Group the descriptors into sets
    // Collect the total size for each type of descriptor
    std::unordered_map<int, std::vector<DescriptorBinding>> sets;
    std::unordered_map<VkDescriptorType, u32> pools;
    for (const auto& binding : bindings) {
        sets[binding.bindingSetLocation].push_back(binding);
        pools[binding.type] += binding.arrayElementCount;
    }

    // Need to alloc a set for each frame in flight
    // and also as much memory in the pool
    u32 numberOfFramesInFlight = frame.framesInFlight();

    // unpack pool sizes
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (const auto& [type, size] : pools) {
        VkDescriptorPoolSize pool;
        pool.type = type;
        pool.descriptorCount = size * numberOfFramesInFlight;

        poolSizes.push_back(pool);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = (u32)poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = sets.size() * numberOfFramesInFlight;

    vk(vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_pool));

    // Create a layout for each set
    for (const auto& [setLocation, set] : sets) {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        for (const auto& layout : set) {
            VkDescriptorSetLayoutBinding bindingLayout{};
            bindingLayout.descriptorType = layout.type;
            bindingLayout.stageFlags = layout.stages;
            bindingLayout.binding = layout.bindingLocation;
            bindingLayout.descriptorCount = layout.arrayElementCount;

            bindings.push_back(bindingLayout);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = bindings.size();
        layoutInfo.pBindings = bindings.data();

        VkDescriptorSetLayout layout;
        vk(vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &layout));

        m_layouts.push_back(layout);
    }

    // Create a set for each layout for each frame in flight
    for (u32 i = 0; i < numberOfFramesInFlight; i++) {
        std::vector<VkDescriptorSet> sets;
        sets.resize(m_layouts.size());

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_pool;
        allocInfo.descriptorSetCount = m_layouts.size();
        allocInfo.pSetLayouts = m_layouts.data();

        vk(vkAllocateDescriptorSets(m_device, &allocInfo, sets.data()));

        m_sets.push_back(sets);
    }
}

DescriptorGroup::~DescriptorGroup() {
    vkDestroyDescriptorPool(m_device, m_pool, nullptr);
    
    for (VkDescriptorSetLayout layout : m_layouts) {
        vkDestroyDescriptorSetLayout(m_device, layout, nullptr);
    }
}

VkDescriptorSetLayout DescriptorGroup::getLayout(int setIndex) {
    return m_layouts[setIndex];
}

VkDescriptorSet DescriptorGroup::getSet(int setIndex) {
    return getSet(m_frame.frameIndex(), setIndex);
}

VkDescriptorSet DescriptorGroup::getSet(int frameIndex, int setIndex) {
    return m_sets[frameIndex][setIndex];
}

void DescriptorGroup::write(int frameIndex, int bindingIndex, Image* image, ImageSampler* sampler) {
    DescriptorBinding binding = m_bindings[bindingIndex];

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = image->getView();
    imageInfo.sampler = sampler->m_sampler;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = getSet(frameIndex, binding.bindingSetLocation);
    write.dstBinding = binding.bindingLocation;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
}

void DescriptorGroup::write(int frameIndex, int bindingIndex, UniformBuffer* uniformBuffer) {
    DescriptorBinding binding = m_bindings[bindingIndex];

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffer->m_buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = uniformBuffer->m_size;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = getSet(frameIndex, binding.bindingSetLocation);
    write.dstBinding = binding.bindingLocation;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
}
