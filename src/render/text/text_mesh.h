#pragma once

#include "asset/font.h"
#include "asset/image.h"
#include "render/backend/render_device.h"

// the unpacked font asset
struct Font {
    Image* msdfAtlas;
    FontAsset data;
};

enum TextAlign {
	// horizontal alignment
	TextAlignLeft,
	TextAlignRight,

	// vertical alignment
	TextAlignTop,
	TextAlignBottom,
	TextAlignBaseline,

	// this is shared
	TextAlignCenter
};

class TextMesh {
public:
    struct Vertex {
        vec2 pos;
        vec2 uv;
    };
    
    static VertexLayout getLayout();

    TextMesh(BufferFactory* bufferFactory, Font* font, u32 maxCharacterCount);
    ~TextMesh();

    void setString(const std::string& text);
    void addString(const std::string& text);

    void removeCharacters(u32 count);

    void draw(CommandBuffer& commandBuffer);

private:
    BufferFactory* bufferFactory;
    Font* font;
    u32 maxCharacterCount;

    Buffer* vertexBuffer;
    std::vector<Vertex> vertices;

    bool needsUpload = false;

    std::string string;

    // Config for just this mesh
    TextAlign horizontalAlign = TextAlignLeft;
    TextAlign verticalAlign = TextAlignBaseline;
    float lineHeightScale = 1.f;
    float characterPaddingX = 0.f;
	float characterPaddingY = 0.f;
	float linePaddingTop = 0.f;
	float linePaddingBottom = 0.f;
	int tabSpaceCount = 4;
};

// class Font {
// public:
//     // could pass image if font asset didn't just store path
//     Font(RenderDevice* device, FontAsset fontAsset, ImageAsset fontImageAsset);
//     ~Font();

//     Font(const Font&) = delete;
//     Font& operator=(const Font&) = delete;

// private:
//     Image* m_image;
//     FontAsset m_font;
// };