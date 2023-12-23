#include "package_input.h"
#include <fstream>
#include <stdexcept>

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

        AssetInput input;
        input.assetNameInPackage = assetName;
        input.inputFile = inputAssetPath;

        if (type == "image") {
            input.type = ASSET_IMAGE;
        } else if (type == "vert" || type == "frag") {
            input.type = ASSET_SHADER_BYTECODE;
        } else if (type == "font") {
            input.type = ASSET_FONT;
        }
        else {
            throw std::runtime_error("Unknown asset type");
        }

        inputs.push_back(input);
    }

    return inputs;
}