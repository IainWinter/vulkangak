#pragma once

#include "render/backend/factory/buffer_factory.h"
#include "render/backend/type/command_buffer.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

class LineMesh {
public:
    struct Vertex {
        vec3 pos;
    };

    struct Point {
        vec2 point;
        float distanceFromStart;
    };

    LineMesh(BufferFactory* bufferFactory, int capResolution = 8);
    ~LineMesh();

    LineMesh(const LineMesh&) = delete;
    LineMesh& operator=(const LineMesh&) = delete;

    void sendToDevice();
    void draw(CommandBuffer& cmd);

    void add(vec2 a);

    float totalLength() const;
    std::vector<Point>& points();

private:
    std::vector<Vertex> m_vertices;
    std::vector<Point> m_points;

    Buffer* m_vertexBuffer;
    Buffer* m_instanceBuffer;

    BufferFactory* m_bufferFactory;

public:
    static VertexLayout getLayout();
};