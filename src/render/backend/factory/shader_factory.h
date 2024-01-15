#pragma once

#include "render/backend/type/shader.h"
#include "render/backend/type/render_pass.h"

class ShaderFactory {
public:
    virtual ~ShaderFactory() = default;

    virtual Shader* createShader(const ShaderProgramSource& source) = 0;
    
    virtual Shader* createShaderRenderPass(RenderPass* renderPass, const ShaderProgramSource& source) = 0;

    virtual void destroyShader(Shader* shader) = 0;
};