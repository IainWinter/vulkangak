#pragma once

#include "render/backend/type/mysettings.h"
#include "render/backend/type/descriptor_set.h"

struct ShaderProgramSource {
    struct ShaderSource {
        ShaderStageBit stageBit;
        std::vector<char> source;
    };

    struct PushConstant {
        ShaderStageBit stageBits;
        size_t size;
    };

    std::vector<ShaderSource> shaders;
    std::vector<PushConstant> pushConstants;
    std::vector<DescriptorSet*> descriptorSets; // this is the only option that requires runtime because the asset compiler doesn't map out all descriptors right now right now
    VertexLayout vertexLayout;
    BlendType blendType;
};

class Shader {
public:
    virtual ~Shader() = default;
};