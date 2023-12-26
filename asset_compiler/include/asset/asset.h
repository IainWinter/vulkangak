#pragma once

#include <string>
#include <vector>

struct AssetMetadata {
    std::string key;
    std::string value;
};

struct Asset {
    std::string path;
    std::vector<AssetMetadata> metadata;
    std::vector<char> binary;

    const std::string& operator[](const std::string& key) const {
        for (const AssetMetadata& metadata : metadata) {
            if (metadata.key == key) {
                return metadata.value;
            }
        }

        throw std::runtime_error("Failed to find metadata");
    }

    int getInt(const std::string& key) const {
        return std::stoi(operator[](key));
    }
    
    float getFloat(const std::string& key) const {
        return std::stof(operator[](key));
    }

    std::string getString(const std::string& key) const {
        return operator[](key);
    }
};