#pragma once

#include "typedef.h"
#include <vector>

enum ImageFormat {
    ImageFormat_RGB_8_SRGB,
    ImageFormat_RGB_8_UNORM,
    ImageFormat_RGBA_8_SRGB,
    ImageFormat_RGBA_8_UNORM,
};

enum ImageLayout {
    ImageLayout_Undefined,
    ImageLayout_TransferDestination,
    ImageLayout_ShaderReadOnly
};

enum ImageSampleCount {
    ImageSampleCount_1,
    ImageSampleCount_2,
    ImageSampleCount_4,
    ImageSampleCount_8,
    ImageSampleCount_16,
    ImageSampleCount_32,
    ImageSampleCount_64,
};

enum BufferType {
    BufferType_TransferSource,
    BufferType_Vertex,
    BufferType_Index,
    BufferType_Uniform,
};

enum ShaderStageBit {
    ShaderStage_Vertex   = 0b01,
    ShaderStage_Fragment = 0b10
};

enum AttributeFormat {
    AttributeFormat_Float,
    AttributeFormat_Float2,
    AttributeFormat_Float3,
    AttributeFormat_Float4,

    AttributeFormat_Int,
};

enum DescriptorType {
    DescriptorType_UniformBuffer,
    DescriptorType_ImageSampler,
};

struct VertexLayout {
    struct Buffer {
        struct Attribute {
            u32 location;
            u32 offset;
            AttributeFormat format;
        };

        u32 binding;
        u32 stride;
        bool instanced;

        std::vector<Attribute> attributes;
    };

    std::vector<Buffer> buffers;
};

struct DescriptorPoolLayout {
    struct Pool {
        DescriptorType type;
        u32 count;
    };

    std::vector<Pool> pools;
    u32 maxSetCount;
};

struct DescriptorSetLayout {
    struct Binding {
        ShaderStageBit stageBits;
        DescriptorType type;
        u32 elementCount;
        u32 location;
    };

    std::vector<Binding> bindings;
};