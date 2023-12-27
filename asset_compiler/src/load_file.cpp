#include "load_file.h"

#include <fstream>

std::vector<char> loadFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to load filepath");
    }
    
    std::streampos size;
    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> bytes;
    
    bytes.resize(size);
    file.read((char*)bytes.data(), size);

    return bytes;
}