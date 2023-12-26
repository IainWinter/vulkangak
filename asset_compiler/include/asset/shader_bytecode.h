#pragma once

#include "asset/asset.h"

struct ShaderBytecodeAsset {
    std::vector<char> bytes;

    ShaderBytecodeAsset() = default;

    ShaderBytecodeAsset(const Asset& asset) {
        bytes = asset.binary;
    }

    Asset createAsset(const std::string& path) {
        Asset asset;
        asset.path = path;
        asset.binary = bytes;

        return asset;
    }
};