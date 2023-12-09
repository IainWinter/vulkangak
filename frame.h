#pragma once

#include "typedef.h"

// For other classes to have info of which frame they are on
// Mainly used for descriptor pool
// Should rename
class FrameSyncInfo {
public:
    FrameSyncInfo(u32* frameIndex, u32* imageIndex, const u32* framesInFlight) 
        : m_frameIndex     (frameIndex)
        , m_imageIndex     (imageIndex) 
        , m_framesInFlight (framesInFlight)
    {}

    u32 frameIndex() const { return *m_frameIndex; }
    u32 imageIndex() const { return *m_imageIndex; }
    u32 framesInFlight() const { return *m_framesInFlight; }
    
private:
    u32* m_frameIndex;
    u32* m_imageIndex;
    const u32* m_framesInFlight;
};
