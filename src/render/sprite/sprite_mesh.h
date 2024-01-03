#pragma once

#include "typedef.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "render/backend/render_device.h"

class SpriteMesh {
public:
    struct Instance {
        vec2 pos = vec2(0, 0);
        vec2 scale = vec2(1, 1);
        float rotation = 0.f;
        vec4 color = vec4(1);

        vec2 uvMin  = vec2(0, 0);
        vec2 uvSize = vec2(1, 1);

        // The texture to use for this instance.
        // 0 is white
        u32 textureIndex = 0;
    };

    struct Vertex {
        vec2 pos;
        vec2 uv;
    };

    static VertexLayout getLayout();

    SpriteMesh(BufferFactory* bufferFactory, u32 batchSize);
    ~SpriteMesh();

    void add(const Instance& instance);
    
    void draw(CommandBuffer& cmd);

private:
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

    BufferFactory* bufferFactory;
    u32 batchSize;

    Buffer* quadBuffer;
    Buffer* indexBuffer;
    Buffer* instanceBuffer;

    std::vector<Instance> instances;
};