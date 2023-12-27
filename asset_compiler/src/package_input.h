#pragma once

#include <vector>
#include <string>

// Load a super simple input file format

// Each asset is defined by 2 consecutive lines

// <asset-type> image       -> stb_image loader
//              vert | frag -> loads spv-5 shader bytecode

// <asset-name>.<asset-type>
// <input file>

enum AssetType {
    ASSET_IMAGE,
    ASSET_FONT,
    ASSET_SHADER_BYTECODE,
    ASSET_SHADER_PROGRAM
};

struct AssetInput {
    AssetType type;
    std::string inputFile;
    std::string assetNameInPackage;
};

std::vector<AssetInput> loadAssetInputs(const std::string& filepath);
