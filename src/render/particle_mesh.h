#pragma once

#include "backend/render_device.h"

template<typename _t>
class ParticleMesh {
public:
    struct Vertex {
        vec2 pos, uv;
    };

    struct Instance {
        vec3 pos;
        vec2 scale;
        float rotation;
        vec4 color;
    };

    ParticleMesh(RenderDevice* device, size_t batchSize) {
        quadBuffer = device->newVertexBuffer<Vertex>(vertices);
        indexBuffer = device->newIndexBuffer(indices);
        instanceBuffer = device->newVertexBuffer(sizeof(Instance), batchSize, nullptr);
    }

    ~ParticleMesh() {
        delete quadBuffer;
        delete indexBuffer;
        delete instanceBuffer;
    }

    void sendToDevice() {
        // If there is no data, skip
        // Nothing will get drawn anyways
        if (instances.size() == 0) {
            return;
        }
        
        instanceBuffer->setData(0, sizeof(Instance) * instances.size(), instances.data());
    }

    void draw(CommandBuffer& cmd) {
        VertexBuffer* buffers[2] = { quadBuffer, instanceBuffer };

        cmd.bindVertexBuffers(2, buffers);
        cmd.bindIndexBuffer(indexBuffer);
        cmd.drawIndexed(indices.size(), instances.size());
    }

    void add(const Instance& instance, const _t& data) {
        addList.push_back({ instance, data });
    }

    void remove(u32 index) {
        removeList.push_back(index);
    }

    void commit() {
        for (const u32& i : removeList) {
            instances.at(i) = instances.back();
            instances.pop_back();

            particleCPUData.at(i) = particleCPUData.back();
            particleCPUData.pop_back();
        }
        removeList.clear();

        // should create more instance buffers
        if (instances.size() + addList.size() > 100) {
            return;
        }

        for (const std::pair<Instance, _t>& p : addList) {
            instances.push_back(p.first);
            particleCPUData.push_back(p.second);
        }
        addList.clear();
    }

    void update(const std::function<void(u32 index, Instance&, _t& data)>& func) {
        for (u32 i = 0; i < instances.size(); i++) {
            func(i, instances[i], particleCPUData[i]);
        }
    }

private:
    // annoying that these need to be vectors
    // there is no reason for that besides the creation api

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {0, 1}}, // 0  2
        {{-0.5f,  0.5f}, {0, 0}}, // 1  3
        {{ 0.5f, -0.5f}, {1, 1}},
        {{ 0.5f,  0.5f}, {1, 0}}
    };

    const std::vector<u32> indices = {
        0, 2, 1,
        1, 2, 3
    };

    VertexBuffer* quadBuffer;
    IndexBuffer*  indexBuffer;

    std::vector<Instance> instances;
    std::vector<_t>       particleCPUData;

    // For now there can only be a single batch
    VertexBuffer* instanceBuffer;

    // Commit list for adding / removing while in update
    std::vector<u32>                     removeList;
    std::vector<std::pair<Instance, _t>> addList;

// Statics

public:
    static std::vector<VulkanVertexLayout> getLayout() {
        return VertexLayoutBuilder()
            .buffer(sizeof(Vertex), false)
                .attribute(0, offsetof(Vertex, pos), VK_FORMAT_R32G32_SFLOAT)
                .attribute(1, offsetof(Vertex, uv),  VK_FORMAT_R32G32_SFLOAT)
            .buffer(sizeof(Instance), true)
                .attribute(2, offsetof(Instance, pos),      VK_FORMAT_R32G32B32_SFLOAT)
                .attribute(3, offsetof(Instance, scale),    VK_FORMAT_R32G32_SFLOAT)
                .attribute(4, offsetof(Instance, rotation), VK_FORMAT_R32_SFLOAT)
                .attribute(5, offsetof(Instance, color),    VK_FORMAT_R32G32B32A32_SFLOAT)
            .build();
    }
};