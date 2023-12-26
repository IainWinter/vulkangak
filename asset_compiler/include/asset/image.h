#pragma once

#include "asset/asset.h"

struct ImageAsset {
    std::vector<char> pixels;
    int width;
    int height;
    int channels;
    bool hasFlippedY;

    ImageAsset() = default;

    ImageAsset(const Asset& asset) {
        pixels = asset.binary;
        width = asset.getInt("width");
        height = asset.getInt("height");
        channels = asset.getInt("channels");
        hasFlippedY = static_cast<bool>(asset.getInt("flipped"));
    }

    Asset createAsset(const std::string& path) {
        Asset asset;
        asset.path = path;
        asset.binary = pixels;
        asset.metadata = {
            { "width",    std::to_string(width) },
            { "height",   std::to_string(height) },
            { "channels", std::to_string(channels) },
            { "flipped",  std::to_string(static_cast<int>(hasFlippedY)) },
        };

        return asset;
    }
};