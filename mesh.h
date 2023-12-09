#pragma once

#include "vertex_buffer.h"
#include "index_buffer.h"

class Mesh {
public:
    Mesh(VertexBuffer* buffer, IndexBuffer* index);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw(VkCommandBuffer commandBuffer);

private:
    VertexBuffer* buffer;
    IndexBuffer* index;
};