#pragma once

#include "asset/asset.h"
#include "asset/structured_binary.h"

struct ShaderProgramAsset {
    enum Stage {
        Stage_Vertex   = 0b01,
        Stage_Fragment = 0b10
    };

    struct PushConstant {
        Stage stage;
        size_t size;
    };

    struct Descriptor {
        enum Type {
            Type_Sampler,
            Type_UniformBuffer
        };

        Stage stages;
        Type type;
        uint32_t elementCount;
        uint32_t location;
        uint32_t set;
    };

    std::vector<char> vertexBytecode;
    std::vector<char> fragmentBytecode;

    std::vector<PushConstant> pushConstants;
    std::vector<Descriptor> descriptors;

    ShaderProgramAsset() = default;

    ShaderProgramAsset(const Asset& asset) {
        StructuredBinary in(asset.binary);

        size_t pushConstantCount;
        size_t descriptorCount;

        in >> vertexBytecode
           >> fragmentBytecode
           >> pushConstantCount
           >> descriptorCount;

        for (size_t i = 0; i < pushConstantCount; i++) {
            PushConstant pushConstant;
            in >> pushConstant.stage
               >> pushConstant.size;

            pushConstants.push_back(pushConstant);
        }

        for (size_t i = 0; i < descriptorCount; i++) {
            Descriptor descriptor;
            in >> descriptor.stages
               >> descriptor.type
               >> descriptor.elementCount
               >> descriptor.location
               >> descriptor.set;

            descriptors.push_back(descriptor);
        }
    }

    Asset createAsset(const std::string& path) {
        StructuredBinary out(100);
        out << vertexBytecode
            << fragmentBytecode
            << pushConstants.size()
            << descriptors.size();

        for (const PushConstant& pushConstant : pushConstants) {
            out << pushConstant.stage 
                << pushConstant.size;
        }

        for (const Descriptor& descriptor : descriptors) {
            out << descriptor.stages
                << descriptor.type
                << descriptor.elementCount
                << descriptor.location
                << descriptor.set;
        }

        Asset asset;
        asset.path = path;
        asset.binary = out.get();

        return asset;
    }
};