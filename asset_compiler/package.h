#pragma once

#include <ostream>
#include <istream>
#include <vector>
#include <string>
#include <fstream>

// This is a super simple binary spec

// number        -> <8 bytes>
// string        -> <n: number> <n bytes>
// AssetMetadata -> <string> <string>
// Asset         -> <string> <n: number> <n AssetMetadatas> <string>
// AssetPackage  -> <n: number> <n Assets>

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

struct AssetPackage {
    std::vector<Asset> assets;

    void add(const Asset& asset) {
        assets.push_back(asset);
    }

    const Asset& operator[](const std::string& path) const {
        for (const Asset& asset : assets) {
            if (asset.path == path) {
                return asset;
            }
        }

        throw std::runtime_error("Failed to find asset");
    }
};

inline void write(std::ostream& out, size_t x) {
    out.write((const char*)&x, sizeof(size_t));
}

inline void write(std::ostream& out, const std::string& x) {
    write(out, x.size());
    out.write(x.c_str(), x.size());
}

inline void write(std::ostream& out, const std::vector<char>& x) {
    write(out, x.size());
    out.write(x.data(), x.size());
}

inline void read(std::istream& in, size_t* x) {
    in.read((char*)x, sizeof(size_t));
}

inline void read(std::istream& in, std::string* x) {
    size_t length;
    read(in, &length);

    x->resize(length);
    in.read((char*)x->data(), length);
}

inline void read(std::istream& in, std::vector<char>* x) {
    size_t length;
    read(in, &length);

    x->resize(length);
    in.read((char*)x->data(), length);
}

inline void writePackage(std::ostream& out, const AssetPackage& package) {
    write(out, package.assets.size());

    for (const Asset& asset : package.assets) {
        write(out, asset.path);
        
        write(out, asset.metadata.size());
        for (const AssetMetadata& metadata : asset.metadata) {
            write(out, metadata.key);
            write(out, metadata.value);
        }

        write(out, asset.binary);
    }
}

inline void readPackage(std::istream& in, AssetPackage* package) {
    size_t assetCount;
    read(in, &assetCount);
    package->assets.resize(assetCount);

    for (size_t i = 0; i < assetCount; i++) {
        Asset& asset = package->assets[i];

        read(in, &asset.path);

        size_t metadataCount;
        read(in, &metadataCount);

        asset.metadata.resize(metadataCount);

        for (size_t j = 0; j < metadataCount; j++) {
            AssetMetadata& metadata = asset.metadata[j];
            read(in, &metadata.key);
            read(in, &metadata.value);
        }

        read(in, &asset.binary);
    }
}

inline AssetPackage loadPackage(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        printf("Failed to load asset package, file not found: %s", filepath.c_str());
        return {};
    }
    
    AssetPackage package;
    readPackage(file, &package);
    return package;
}