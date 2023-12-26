#include "shader_bytecode.h"

#include "asset/shader_bytecode.h"

void loadShaderByteCode(AssetPackage* package, const std::string& path, const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to load filepath");
    }
    
    std::streampos size;
    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    ShaderBytecodeAsset shader;
    shader.bytes.resize(size);
    file.read((char*)shader.bytes.data(), size);

    package->add(shader.createAsset(path));
}