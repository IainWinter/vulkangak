#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>

#include "package_input.h"

#include "load/font.h"
#include "load/image.h"
#include "load/shader_bytecode.h"
#include "load/shader_program.h"

#include <filesystem>

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

    // make sure all files exist
    for (const AssetInput& input: assets) {
        if (!std::filesystem::exists(input.inputFile)) {
            std::cout << "File does not exist: " << input.inputFile << std::endl;
            std::cout << "Exiting...";
            return 0;
        }
    }

    for (const AssetInput& input : assets) {
        std::cout << "\t" << input.assetNameInPackage << " <- " << input.inputFile << std::endl;

        switch (input.type) {
            case ASSET_IMAGE: {
                loadImage(&package, input.assetNameInPackage, input.inputFile);
                break;
            }
            case ASSET_SHADER_BYTECODE: {
                loadShaderByteCode(&package, input.assetNameInPackage, input.inputFile);
                break;
            }
            case ASSET_FONT: {
                loadFont(&package, input.assetNameInPackage, input.inputFile, 32.0f);
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