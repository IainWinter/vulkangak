#pragma once

#include "asset/asset.h"
#include "asset/structured_binary.h"

#include "glm/vec2.hpp"
using namespace glm;

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

    FontAsset() = default;

	FontAsset(const Asset& asset) {
		spaceAdvance = asset.getFloat("spaceAdvance");
		lineHeight = asset.getFloat("lineHeight");
		topHeight = asset.getFloat("topHeight");
		bottomHeight = asset.getFloat("bottomHeight");
		atlasPath = asset.getString("atlas");

		StructuredBinary binary(asset.binary);

		glyphs.resize(asset.getInt("glyphCount"));
		kerning.resize(asset.getInt("kerningCount"));

		for (FontAsset::Glyph& glyph : glyphs) {
			binary >> glyph.posMin
				>> glyph.posMax
				>> glyph.uvMin
				>> glyph.uvMax
				>> glyph.advance
				>> glyph.index
				>> glyph.character;
		}

		for (FontAsset::Kerning& kerning : kerning) {
			binary >> kerning.left
				>> kerning.right
				>> kerning.amount;
		}
    }

    Asset createAsset(const std::string& path) {
		size_t size = sizeof(FontAsset::Glyph) * glyphs.size()
					+ sizeof(FontAsset::Kerning) * kerning.size();

		StructuredBinary binary(size);

		for (const FontAsset::Glyph& glyph : glyphs) {
			binary << glyph.posMin
				<< glyph.posMax
				<< glyph.uvMin
				<< glyph.uvMax
				<< glyph.advance
				<< glyph.index
				<< glyph.character;
		}

		for (const FontAsset::Kerning& kerning : kerning) {
			binary << kerning.left
				<< kerning.right
				<< kerning.amount;
		}

		Asset asset;
		asset.path = path;   
		asset.binary = binary.get();

		asset.metadata = {
			{ "glyphCount",   std::to_string(glyphs.size()) },
			{ "kerningCount", std::to_string(kerning.size()) },
			{ "spaceAdvance", std::to_string(spaceAdvance) },
			{ "lineHeight",   std::to_string(lineHeight) },
			{ "topHeight",    std::to_string(topHeight) },
			{ "bottomHeight", std::to_string(bottomHeight) },
			{ "atlas",        atlasPath },
		};

		return asset;
    }

	Glyph getGlyph(int character) const {
		for (const auto& glyph : glyphs) {
			if (glyph.character == character) {
				return glyph;
			}
		}

		return {};
	}

	float getKerning(int leftCharacter, int rightCharacter) const {
		for (const auto& kerning : kerning) {
			if (kerning.left == leftCharacter && kerning.right == rightCharacter) {
				return kerning.amount;
			}
		}

		return 0.0f;
	}
};