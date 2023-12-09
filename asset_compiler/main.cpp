#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>

#include "package.h"
#include "package_input.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Asset loadImage(const std::string& path, const std::string& filepath) {
	int width, height, channels, format;
	stbi_info(filepath.c_str(), &width, &height, &channels);

	stbi_set_flip_vertically_on_load(true);

	switch (channels)
	{
        case 1: format = STBI_grey;       break;
        case 2: format = STBI_grey_alpha; break;
        case 3: format = STBI_rgb;        break;
        case 4: format = STBI_rgb_alpha;  break;
        default:
            throw std::runtime_error("Failed to load image, invalid number of channels");
	}

	char* pixels = (char*)stbi_load(filepath.c_str(), &width, &height, &channels, format);

	if (!pixels) {
        throw std::runtime_error("Failed to load image, no pixels");
	}

    Asset asset;
    asset.path = path;
    asset.metadata.push_back({ "width", std::to_string(width) });
    asset.metadata.push_back({ "height", std::to_string(height) });
    asset.metadata.push_back({ "channels", std::to_string(channels) });
    asset.metadata.push_back({ "flipped", std::to_string(1) });
    asset.binary = std::vector<char>(pixels, pixels + width * height * channels);

    return asset;
}

Asset loadShaderByteCode(const std::string& path, const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to load filepath");
    }
    
    std::streampos size;
    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    Asset asset;
    asset.path = path;

    asset.binary.resize(size);
    file.read((char*)asset.binary.data(), size);

    return asset;
}

int main(int argc, const char** argv) {
    if (argc < 3) {
        std::cout << "Usage: <input_file> <output_file>" << std::endl;
        return 1;
    }

    const char* input = argv[1];
    const char* output = argv[2];

    AssetPackage package;
    std::vector<AssetInput> assets = loadAssetInputs(input);

    std::cout << "Compiling assets:" << std::endl;

    for (const AssetInput& input : assets) {
        std::cout << "\t" << input.assetNameInPackage << " <- " << input.inputFile << std::endl;

        switch (input.type) {
            case ASSET_INPUT: {
                package.assets.push_back(loadImage(input.assetNameInPackage, input.inputFile));
                break;
            }
            case ASSET_SHADER_BYTECODE: {
                package.assets.push_back(loadShaderByteCode(input.assetNameInPackage, input.inputFile));
                break;
            }
            default: {
                throw std::runtime_error("Unknown asset type");
            }
        }
    }

    // If write out
    {
        std::ofstream out(output, std::ios::binary);
        writePackage(out, package);
    }

    // AssetPackage packageIn;
    // {
    //     std::ifstream in("assets.bin", std::ios::binary);
    //     readPackage(in, &packageIn);
    // }
}