#include "render/sprite/sprite_mesh.h"
#include "containers/pop_erase.h"

VertexLayout SpriteMesh::getLayout() {
    return {{
        {
            0, sizeof(Vertex), false, {
                { 0, offsetof(Vertex, pos), AttributeFormat_Float2 },
                { 1, offsetof(Vertex, uv), AttributeFormat_Float2 },
            }
        },
        {
            1, sizeof(Instance), true, {
                { 2, offsetof(Instance, pos), AttributeFormat_Float2 },
                { 3, offsetof(Instance, scale), AttributeFormat_Float2 },
                { 4, offsetof(Instance, rotation), AttributeFormat_Float },
                { 5, offsetof(Instance, color), AttributeFormat_Float4 },
                { 6, offsetof(Instance, uvMin), AttributeFormat_Float2 },
                { 7, offsetof(Instance, uvSize), AttributeFormat_Float2 },
                { 8, offsetof(Instance, textureIndex), AttributeFormat_Int },
            }
        }
    }};
}

SpriteMesh::SpriteMesh(BufferFactory* bufferFactory, u32 batchSize) 
    : bufferFactory (bufferFactory)
    , batchSize     (batchSize)
{
    quadBuffer = bufferFactory->createVertexBufferFromVector(vertices);
    indexBuffer = bufferFactory->createIndexBufferFromVector(indices);
    instanceBuffer = bufferFactory->createVertexBufferEmpty(sizeof(Instance), batchSize);
}

SpriteMesh::~SpriteMesh() {
    bufferFactory->destroyBuffer(quadBuffer);
    bufferFactory->destroyBuffer(indexBuffer);
    bufferFactory->destroyBuffer(instanceBuffer);
}

void SpriteMesh::add(const Instance& instance) {
    if (instances.size() + 1 >= batchSize) {
        return;
    }

    instances.push_back(instance);
}

void SpriteMesh::draw(CommandBuffer& cmd) {
    if (instances.size() == 0) {
        return;
    }

    instanceBuffer->setDataVector(instances);

    Buffer* buffers[2] = { quadBuffer, instanceBuffer };

    cmd.bindVertexBuffers(2, buffers);
    cmd.bindIndexBuffer(indexBuffer);
    cmd.drawIndexed(indices.size(), instances.size());
}