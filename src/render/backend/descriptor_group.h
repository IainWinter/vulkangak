#pragma once

#include "frame.h"
#include "image.h"
#include "uniform_buffer.h"

#include <vector>

// Seems like these need to be pre alloced and be double allocated for each frame

struct DescriptorBinding {
    VkDescriptorType type;
    VkShaderStageFlagBits stages;
    u32 arrayElementCount;
    u32 bindingLocation;
    u32 bindingSetLocation;
};

class DescriptorGroup {
public:
    DescriptorGroup(VkDevice device, FrameSyncInfo frame, const std::vector<DescriptorBinding>& bindings);
    ~DescriptorGroup();

    VkDescriptorSetLayout getLayout(int setIndex);

    VkDescriptorSet getSet(int setIndex);
    VkDescriptorSet getSet(int frameIndex, int setIndex);

    void write(int frameIndex, int bindingIndex, Image* image, ImageSampler* sampler);
    void write(int frameIndex, int bindingIndex, UniformBuffer* uniformBuffer);

private:
    VkDevice m_device;
    FrameSyncInfo m_frame;
    std::vector<DescriptorBinding> m_bindings; // [binding index]

    VkDescriptorPool m_pool;
    std::vector<VkDescriptorSetLayout> m_layouts;     // [set index]
    std::vector<std::vector<VkDescriptorSet>> m_sets; // [frame][set index]
};