#pragma once

#include "render/backend/type/mysettings.h"
#include "render/backend/type/descriptor_set_layout.h"

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
    std::vector<DescriptorSetLayout*> descriptorSetLayouts; // this a runtime resource, but all data to create it can be known through an asset.
    VertexLayout vertexLayout;                              // so this entire struct can now be an asset called ShaderProgramAsset :)
    BlendType blendType;
};

class Shader {
public:
    virtual ~Shader() = default;
};