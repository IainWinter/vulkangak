#include "asset_types.h"
#include "structured_binary.h"

#include <sstream>

Asset createImageAsset(const std::string& path, const ImageAsset& image) {
    size_t size = image.width * image.height * image.channels;

    Asset asset;
    asset.path = path;
    asset.binary = image.pixels;
    asset.metadata = {
        { "width",    std::to_string(image.width) },
        { "height",   std::to_string(image.height) },
        { "channels", std::to_string(image.channels) },
        { "flipped",  std::to_string(static_cast<int>(image.hasFlippedY)) },
    };

    return asset;
}

ImageAsset parseImageAsset(const Asset& asset) {
    ImageAsset image;
    image.pixels = asset.binary;
    image.width = asset.getInt("width");
    image.height = asset.getInt("height");
    image.channels = asset.getInt("channels");
    image.hasFlippedY = static_cast<bool>(asset.getInt("flipped"));

    return image;
}

Asset createShaderByteCodeAsset(const std::string& path, const ShaderByteCodeAsset& shaderByteCode) {
    Asset asset;
    asset.path = path;
    asset.binary = shaderByteCode.bytes;

    return asset;
}

ShaderByteCodeAsset parseShaderByteCodeAsset(const Asset& asset) {
    ShaderByteCodeAsset shaderByteCode;
    shaderByteCode.bytes = asset.binary;

    return shaderByteCode;
}

Asset createFontAsset(const std::string& path, const FontAsset& font) {
    size_t size = sizeof(FontAsset::Glyph) * font.glyphs.size();
                + sizeof(FontAsset::Kerning) * font.kerning.size();

    StructuredBinary binary(size);

    for (const FontAsset::Glyph& glyph : font.glyphs) {
        binary << glyph.posMin
               << glyph.posMax
               << glyph.uvMin
               << glyph.uvMax
               << glyph.advance
               << glyph.index
               << glyph.character;
    }

    for (const FontAsset::Kerning& kerning : font.kerning) {
        binary << kerning.left
               << kerning.right
               << kerning.amount;
    }

    Asset asset;
    asset.path = path;   
    asset.binary = binary.get();

    asset.metadata = {
        { "glyphCount",   std::to_string(font.glyphs.size()) },
        { "kerningCount", std::to_string(font.kerning.size()) },
        { "spaceAdvance", std::to_string(font.spaceAdvance) },
        { "lineHeight",   std::to_string(font.lineHeight) },
        { "topHeight",    std::to_string(font.topHeight) },
        { "bottomHeight", std::to_string(font.bottomHeight) },
        { "atlas",        font.atlasPath },
    };

    return asset;
}

FontAsset parseFontAsset(const Asset& asset) {
    FontAsset font;
    font.spaceAdvance = asset.getInt("spaceAdvance");
    font.lineHeight = asset.getInt("lineHeight");
    font.topHeight = asset.getInt("topHeight");
    font.bottomHeight = asset.getInt("bottomHeight");
    font.atlasPath = asset.getString("atlas");

    StructuredBinary binary(asset.binary);

    font.glyphs.resize(asset.getInt("glyphsCount"));
    font.kerning.resize(asset.getInt("kerningCount"));

    for (FontAsset::Glyph& glyph : font.glyphs) {
        binary >> glyph.posMin
               >> glyph.posMax
               >> glyph.uvMin
               >> glyph.uvMax
               >> glyph.advance
               >> glyph.index
               >> glyph.character;
    }

    for (FontAsset::Kerning& kerning : font.kerning) {
        binary >> kerning.left
               >> kerning.right
               >> kerning.amount;
    }

    return font;
}