#pragma once

#include "asset/font.h"
#include "asset/image.h"
#include "render/backend/render_device.h"

class TextMesh {
public:
    struct Vertex {
        vec2 pos;
        vec2 uv;
    };
    
    static VertexLayout getLayout();

    TextMesh(BufferFactory* bufferFactory, FontAsset fontAsset);

private:

};

class Font {
public:
    // could pass image if font asset didn't just store path
    Font(RenderDevice* device, FontAsset fontAsset, ImageAsset fontImageAsset);
    ~Font();

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    TextMesh* createTextMesh(const std::string& text, float size);
    TextMesh* createTextMeshMaxCharacters(const std::string& text, float size, u32 maxCharacters);

private:
    Image* m_image;
    FontAsset m_font;
};