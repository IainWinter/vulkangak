#pragma once

#include "render/backend/type/render_pass.h"

class RenderPassFactory {
public:
    virtual ~RenderPassFactory() = default;

    virtual RenderPass* createRenderPass() = 0;

    virtual void destroyRenderPass(RenderPass* renderPass) = 0;
};