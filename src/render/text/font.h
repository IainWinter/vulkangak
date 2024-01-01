#pragma once

#include "asset/font.h"
#include "asset/image.h"
#include "render/backend/render_device.h"

class Font {
public:
    // could pass image if font asset didn't just store path
    Font(RenderDevice* device, FontAsset fontAsset, ImageAsset fontImageAsset);
    ~Font();

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    

private:
    Image* m_image;

    FontAsset m_font;
};