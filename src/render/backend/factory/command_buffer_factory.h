#pragma once

#include "render/backend/type/command_buffer.h"
#include "typedef.h"

class CommandBufferFactory {
public:
    virtual ~CommandBufferFactory() = default;

    virtual CommandBuffer* createCommandBuffer() = 0;

    virtual void destroyCommandBuffer(CommandBuffer* commandBuffer) = 0;
};