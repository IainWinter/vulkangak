#pragma once

#include <vector>
#include <string>

#include "glm/vec2.hpp"
using namespace glm;

class StructuredBinary {
public:
    StructuredBinary(size_t initialSize) 
        : bytes     ()
        , readIndex (0)
    {
        bytes.reserve(initialSize);
    }

    StructuredBinary(std::vector<char> binary) 
        : bytes     (binary)
        , readIndex (0)
    {}

    // writing

    void writeFloat(float x) {
        char* ptr = reinterpret_cast<char*>(&x);
        bytes.insert(bytes.end(), ptr, ptr + sizeof(float));
    }

    void writeFloat2(const vec2& v) {
        writeFloat(v.x);
        writeFloat(v.y);
    }

    void writeInt(int x) {
        char* ptr = reinterpret_cast<char*>(&x);
        bytes.insert(bytes.end(), ptr, ptr + sizeof(int));
    }

    void writeString(const std::string& str) {
        writeInt(str.size());
        bytes.insert(bytes.end(), str.begin(), str.end());
    }

    void writeBinary(const std::vector<char>& binary) {
        writeInt(binary.size());
        bytes.insert(bytes.end(), binary.begin(), binary.end());
    }

    const std::vector<char>& get() const {
        return bytes;
    }

    template<typename _t>
    StructuredBinary& operator<<(const _t& x) {
             if constexpr (std::is_same_v<_t, float>)             writeFloat(x);
        else if constexpr (std::is_same_v<_t, vec2>)              writeFloat2(x);
        else if constexpr (std::is_same_v<_t, int>)               writeInt(x);
        else if constexpr (std::is_same_v<_t, std::string>)       writeString(x);
        else if constexpr (std::is_same_v<_t, std::vector<char>>) writeBinary(x);

        return *this;
    }

    // reading

    void readFloat(float* x) {
        char* ptr = reinterpret_cast<char*>(x);
        std::copy(bytes.begin() + readIndex, bytes.begin() + readIndex + sizeof(float), ptr);
        readIndex += sizeof(float);
    }

    void readFloat2(vec2* v) {
        readFloat(&v->x);
        readFloat(&v->y);
    }

    void readInt(int* x) {
        char* ptr = reinterpret_cast<char*>(x);
        std::copy(bytes.begin() + readIndex, bytes.begin() + readIndex + sizeof(int), ptr);
        readIndex += sizeof(int);
    }

    void readString(std::string* str) {
        int size;
        readInt(&size);

        str->resize(size);
        std::copy(bytes.begin() + readIndex, bytes.begin() + readIndex + size, str->begin());
        readIndex += size;
    }

    void readBinary(std::vector<char>* binary) {
        int size;
        readInt(&size);

        binary->resize(size);
        std::copy(bytes.begin() + readIndex, bytes.begin() + readIndex + size, binary->begin());
        readIndex += size;
    }

    template<typename _t>
    StructuredBinary& operator>>(_t& x) {
             if constexpr (std::is_same_v<_t, float>)             readFloat(&x);
        else if constexpr (std::is_same_v<_t, vec2>)              readFloat2(&x);
        else if constexpr (std::is_same_v<_t, int>)               readInt(&x);
        else if constexpr (std::is_same_v<_t, std::string>)       readString(&x);
        else if constexpr (std::is_same_v<_t, std::vector<char>>) readBinary(&x);

        return *this;
    }

private:
    std::vector<char> bytes;
    size_t readIndex = 0;
};