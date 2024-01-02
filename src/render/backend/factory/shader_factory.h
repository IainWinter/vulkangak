#pragma once

#include "render/backend/type/shader.h"

class ShaderFactory {
public:
    virtual ~ShaderFactory() = default;

    virtual Shader* createShader(const ShaderProgramSource& source) = 0;

    virtual void destroyShader(Shader* shader) = 0;
};