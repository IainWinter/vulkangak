#pragma once

#include "typedef.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "render/backend/render_device.h"

class SpriteMesh {
public:
    struct Instance {
        vec3 pos;
        vec2 scale;
        vec3 rotation;
        vec4 color;

        vec2 uvMin;
        vec2 uvMax;

        // do not set this, it is set by the mesh
        int id_internal;
    };

    struct Vertex {
        vec2 pos, uv;
    };

    SpriteMesh();
    ~SpriteMesh();

    u32 add(const Instance& instance);
    void remove(u32 id);

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

    VertexBuffer* quadBuffer;
    IndexBuffer*  indexBuffer;

    std::vector<Instance> instances;

    VertexBuffer* instanceBuffer;
};