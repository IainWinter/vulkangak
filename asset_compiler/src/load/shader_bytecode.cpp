#include "shader_bytecode.h"
#include "load_file.h"

#include "asset/shader_bytecode.h"

void loadShaderByteCode(AssetPackage* package, const std::string& path, const std::string& filepath) {
    ShaderBytecodeAsset shader;
    shader.bytes = loadFile(filepath);

    package->add(shader.createAsset(path));
}