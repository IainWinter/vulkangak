#pragma once

#include "render/backend/type/mysettings.h"
#include "typedef.h"

class Buffer {
public:
    virtual ~Buffer() = default;

    virtual void setData(const void* data) = 0;
    virtual void setData(u32 offset, u32 size, const void* data) = 0;

    virtual void* map() = 0;
    virtual void* map(u32 offset, u32 size) = 0;

    virtual void unmap() = 0;
};