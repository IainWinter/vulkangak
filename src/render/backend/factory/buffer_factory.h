#pragma once

#include "render/backend/type/buffer.h"
#include <vector>

class BufferFactory {
public:
    virtual ~BufferFactory() = default;

    virtual Buffer* createBufferEmpty(BufferType type, size_t size) = 0;
    virtual Buffer* createBuffer(BufferType type, size_t size, const u8* data) = 0;

    virtual Buffer* createVertexBufferEmpty(size_t vertexSize, size_t vertexCount) = 0;
    virtual Buffer* createVertexBuffer(size_t vertexSize, size_t vertexCount, const u8* vertexData) = 0;

    virtual Buffer* createIndexBufferEmpty(size_t indexCount) = 0;
    virtual Buffer* createIndexBuffer(size_t indexCount, const u32* indexData) = 0;

    virtual Buffer* createUniformBufferEmpty(size_t size) = 0;

    virtual void destroyBuffer(Buffer* buffer) = 0;

    template<typename _t>
    Buffer* createVertexBufferFromVector(const std::vector<_t>& vertices) {
        if (vertices.size() == 0) {
            return createVertexBufferEmpty(sizeof(_t), 0);
        }

        return createVertexBuffer(sizeof(_t), vertices.size(), (const u8*)vertices.data());
    }

    inline Buffer* createIndexBufferFromVector(const std::vector<u32> indices) {
        if (indices.size() == 0) {
            return createVertexBufferEmpty(sizeof(u32), 0);
        }

        return createIndexBuffer(indices.size(), indices.data());
    }

    template<typename _t>
    Buffer* createUniformBufferEmptyFromType() {
        return createUniformBufferEmpty(sizeof(_t));
    }
};