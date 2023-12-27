#include "shader_program.h"
#include "load_file.h"

#include "asset/shader_program.h"

#include "glm/mat4x4.hpp"
using namespace glm;

void loadShaderProgram(AssetPackage* package, const std::string& path, const std::string& filepath) {
    ShaderProgramAsset program;

    program.pushConstants = {
        ShaderProgramAsset::PushConstant {
            .stage = ShaderProgramAsset::Stage_Vertex,
            .size = sizeof(mat4)
        }
    };

    program.descriptors = {
        ShaderProgramAsset::Descriptor {
            .type = ShaderProgramAsset::Descriptor::Type_UniformBuffer,
            .stages = ShaderProgramAsset::Stage_Vertex,
            .elementCount = 1,
            .location = 0,
            .set = 0
        },
        ShaderProgramAsset::Descriptor {
            .type = ShaderProgramAsset::Descriptor::Type_Sampler,
            .stages = ShaderProgramAsset::Stage_Fragment,
            .elementCount = 1,
            .location = 1,
            .set = 0
        }
    };

    program.vertexBytecode = loadFile("./build/shaders/vert.spv");
    program.fragmentBytecode = loadFile("./build/shaders/frag.spv");

    package->add(program.createAsset(path));
}