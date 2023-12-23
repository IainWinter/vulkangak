#pragma once

#include "package.h"

#include "glm/vec2.hpp"
using namespace glm;

// These are functions which create assets from structs

struct ImageAsset {
    std::vector<char> pixels;
    int width;
    int height;
    int channels;
    bool hasFlippedY;
};

Asset createImageAsset(const std::string& path, const ImageAsset& image);
ImageAsset parseImageAsset(const Asset& asset);

struct ShaderByteCodeAsset {
    std::vector<char> bytes;
};

Asset createShaderByteCodeAsset(const std::string& path, const ShaderByteCodeAsset& shaderByteCode);
ShaderByteCodeAsset parseShaderByteCodeAsset(const Asset& asset);

struct FontAsset {
	struct Glyph {
		vec2 posMin;
		vec2 posMax;
		vec2 uvMin;
		vec2 uvMax;

		float advance;
		int index;
		int character;
	};

	struct Kerning {
		int left;
		int right;
		float amount;
	};

    std::string atlasPath;
	
	std::vector<Glyph> glyphs;
	std::vector<Kerning> kerning;
	
	float spaceAdvance;
	float lineHeight;
	float topHeight;
	float bottomHeight;

	const Glyph& getGlyph(int character) const {
		for (const auto& glyph : glyphs) {
			if (glyph.character == character) {
				return glyph;
			}
		}

		return {};
	}
};

Asset createFontAsset(const std::string& path, const FontAsset& font);
FontAsset parseFontAsset(const Asset& asset);