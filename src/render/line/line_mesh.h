#pragma once

#include "render/backend/render_device.h"

class LineMesh {
public:
    struct Vertex {
        vec3 pos;
    };

    struct Point {
        vec2 point;
        float distanceFromStart;
    };

    LineMesh(RenderDevice* device, int capResolution = 8);
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

    VertexBuffer* m_vertexBuffer;
    VertexBuffer* m_instanceBuffer;

public:
    static std::vector<VulkanVertexLayout> getLayout();
};