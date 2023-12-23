#include "package_input.h"
#include <fstream>
#include <stdexcept>
#include <unordered_map>

static const std::unordered_map<std::string, AssetType> s_assetTypes = {
    { "image", ASSET_IMAGE },
    { "vert", ASSET_SHADER_BYTECODE },
    { "frag", ASSET_SHADER_BYTECODE },
    { "font", ASSET_FONT },
};

std::vector<AssetInput> loadAssetInputs(const std::string& filepath) {
    std::vector<AssetInput> inputs;

    std::string assetName;
    std::string inputAssetPath;
    
    std::ifstream in(filepath);
    while (std::getline(in, assetName)) {
        // Skip newlines
        if (assetName == "") {
            continue;
        }

        std::getline(in, inputAssetPath);

        std::string type = assetName.substr(assetName.find_last_of('.') + 1);

        auto itr = s_assetTypes.find(type);
        if (itr == s_assetTypes.end()) {
            throw std::runtime_error("Unknown asset type");
        }

        AssetInput input;
        input.assetNameInPackage = assetName;
        input.inputFile = inputAssetPath;
        input.type = itr->second;

        inputs.push_back(input);
    }

    return inputs;
}