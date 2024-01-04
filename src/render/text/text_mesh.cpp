#include "render/text/text_mesh.h"
#include "glm/common.hpp"

VertexLayout TextMesh::getLayout() {
    VertexLayout vertexLayout;

    VertexLayout::Buffer vertexBuffer{};
    vertexBuffer.stride = sizeof(Vertex);
    vertexBuffer.attributes = {
        { 0, offsetof(Vertex, pos), AttributeFormat_Float2 },
        { 1, offsetof(Vertex, uv), AttributeFormat_Float2 }
    };

    vertexLayout.buffers = { vertexBuffer };

    return vertexLayout;
}

TextMesh::TextMesh(BufferFactory* bufferFactory, Font* font, u32 maxCharacterCount) 
    : bufferFactory     (bufferFactory)
    , font              (font)
    , maxCharacterCount (maxCharacterCount) 
{
    vertexBuffer = bufferFactory->createVertexBufferEmpty(sizeof(Vertex), maxCharacterCount * 6);
}

TextMesh::~TextMesh() {
    bufferFactory->destroyBuffer(vertexBuffer);
}

void TextMesh::setString(const std::string& text) {
    string = "";
    vertices = {};
    addString(text);
}

void TextMesh::addString(const std::string& text) {
    string += text;

    struct Line {
        struct GlyphBounds {
            vec2 posMin;
            vec2 posMax;
            vec2 uvMin;
            vec2 uvMax;
        };

        std::vector<GlyphBounds> glyphs;
        float width;
    };

    std::vector<Line> lines;

    Line line = {};
    vec2 cursor = vec2(0, 0);

    for (int i = 0; i < text.length(); i++) {
        char c = text[i];

        if (c == '\r') {
            // just ignore because it wont overwrite the line, like in a console
            //cursor.x = 0;
            continue;
        }

        if (c == '\n') {
            lines.push_back(line);
            line = {};

            cursor.x = 0;
            cursor.y -= font->data.lineHeight * lineHeightScale + characterPaddingY;
            continue;
        }

        if (c == ' ') {
            float kerning = (i + 1 != text.length()) 
                ? font->data.getKerning(' ', text[i + 1])
                : 0.0f;

            cursor.x += font->data.spaceAdvance + kerning + characterPaddingX;
            continue;
        }

        if (c == '\t') {
            cursor.x += tabSpaceCount * font->data.spaceAdvance + characterPaddingX;
            continue;
        }
        
        FontAsset::Glyph glyph = font->data.getGlyph(text[i]);

        if (glyph.index == -1) {
            // should long
            continue;
        }

        Line::GlyphBounds bounds;
        bounds.posMin = (glyph.posMin + cursor) * font->data.lineHeight;
        bounds.posMax = (glyph.posMax + cursor) * font->data.lineHeight;
        bounds.uvMin = glyph.uvMin;
        bounds.uvMax = glyph.uvMax;

        line.glyphs.push_back(bounds);
        line.width = max(line.width, bounds.posMax.x);

        // no need to advance for the last character
        if (i + 1 == text.length()) {
            continue;
        }

        cursor.x += glyph.advance + font->data.getKerning(text[i], text[i + 1]) + characterPaddingX;
    }

    lines.push_back(line);

    vec2 alignmentCursor = vec2(0, 0);
    float totalHeight = font->data.lineHeight * (lines.size() - 1);

    switch (verticalAlign) {
        case TextAlignTop:
            alignmentCursor.y = -font->data.topHeight;
            break;
        case TextAlignBottom:
            alignmentCursor.y = totalHeight + font->data.bottomHeight;
            break;
        case TextAlignCenter:
            alignmentCursor.y = totalHeight / 2.f - font->data.bottomHeight;
            break;
        case TextAlignBaseline:
            alignmentCursor.y = 0;
            break;
        default:
            break;
    }

    for (const Line& line : lines) {
        switch (horizontalAlign) {
            case TextAlignLeft: 
			    alignmentCursor.x = 0;
			    break;
            case TextAlignCenter:
                alignmentCursor.x = -line.width / 2;
				break;
            case TextAlignRight:
                alignmentCursor.x = -line.width;
                break;
            default:
                break;
        }

        for (const Line::GlyphBounds& bounds : line.glyphs) {
            Vertex v0 = {};
            v0.pos = bounds.posMin + alignmentCursor;
            v0.uv = bounds.uvMin;

            Vertex v1 = {};
            v1.pos = vec2(bounds.posMin.x, bounds.posMax.y) + alignmentCursor;
            v1.uv = vec2(bounds.uvMin.x, bounds.uvMax.y);

            Vertex v2 = {};
            v2.pos = vec2(bounds.posMax.x, bounds.posMin.y) + alignmentCursor;
            v2.uv = bounds.uvMax;

            Vertex v3 = {};
            v3.pos = bounds.posMax + alignmentCursor;
            v3.uv = vec2(bounds.uvMax.x, bounds.uvMin.y);

            vertices.push_back(v0);
            vertices.push_back(v2);
            vertices.push_back(v1);

            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
        }
    }

    needsUpload = true;
}

void TextMesh::removeCharacters(u32 count) {
    count = min(count, (u32)string.length());

    for (u32 i = 0; i < count; i++) {
        u32 j = string.length() - 1 - i;

        // skip whitespace
        if (string[j] != ' ' || string[j] != '\t') {
            continue;
        }

        vertices.pop_back();
        vertices.pop_back();
        vertices.pop_back();
        vertices.pop_back();
    }

    needsUpload = true;
}

void TextMesh::draw(CommandBuffer& commandBuffer) {
    if (needsUpload) {
        vertexBuffer->setDataVector(vertices);
        needsUpload = false;
    }

    if (vertices.size() == 0) {
        return;
    }

    commandBuffer.bindVertexBuffers(1, &vertexBuffer);
    commandBuffer.draw(vertices.size(), 1);
}