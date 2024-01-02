#include "line_mesh.h"

#include "glm/geometric.hpp"

LineMesh::LineMesh(BufferFactory* bufferFactory, int capResolution) 
    : m_bufferFactory (bufferFactory)
{
    // https://wwwtyro.net/2019/11/18/instanced-lines.html

    m_vertices = {
        {{ 0, -0.5, 0 }},
        {{ 0, -0.5, 1 }},
        {{ 0,  0.5, 1 }},
        {{ 0, -0.5, 0 }},
        {{ 0,  0.5, 1 }},
        {{ 0,  0.5, 0 }}
    };

    // add the end point to the vertices

    for (int step = 0; step < capResolution; step++) {
        float theta0 = PI / 2 + ((step + 0) * PI) / capResolution;
        float theta1 = PI / 2 + ((step + 1) * PI) / capResolution;
        m_vertices.push_back({{ 0, 0, 0 }});
        m_vertices.push_back({{ .5f * cos(theta0), .5f * sin(theta0), 0}});
        m_vertices.push_back({{ .5f * cos(theta1), .5f * sin(theta1), 0}});
    }

    for (int step = 0; step < capResolution; step++) {
        float theta0 = (3 * PI) / 2 + ((step + 0) * PI) / capResolution;
        float theta1 = (3 * PI) / 2 + ((step + 1) * PI) / capResolution;
        m_vertices.push_back({{ 0, 0, 1 }});
        m_vertices.push_back({{ .5f * cos(theta0), .5f * sin(theta0), 1}});
        m_vertices.push_back({{ .5f * cos(theta1), .5f * sin(theta1), 1}});
    }

    m_vertexBuffer = bufferFactory->createVertexBufferFromVector(m_vertices);
    m_instanceBuffer = bufferFactory->createVertexBufferEmpty(sizeof(Point), 100);
}

LineMesh::~LineMesh() {
    m_bufferFactory->destroyBuffer(m_vertexBuffer);
    m_bufferFactory->destroyBuffer(m_instanceBuffer);
}

void LineMesh::sendToDevice() {
    if (m_points.size() <= 1) {
        return;
    }

    // recalc distance from start
    // dumb to to this every time
    // m_points[0].distanceFromStart = 0;
    // for (int i = 1; i < m_points.size(); i++) {
    //     const Point& a = m_points[i - 1];
    //     Point& b = m_points[i];

    //     b.distanceFromStart = a.distanceFromStart + distance(a.point, b.point);
    // }

    m_instanceBuffer->setData(0, sizeof(Point) * m_points.size(), m_points.data());
}

void LineMesh::draw(CommandBuffer& cmd) {
    if (m_points.size() <= 1) {
        return;
    }
    
    Buffer* buffers[2] = { m_vertexBuffer, m_instanceBuffer };

    cmd.bindVertexBuffers(2, buffers);
    cmd.draw(m_vertices.size(), m_points.size() - 1);
}

void LineMesh::add(vec2 a) {
    float distanceFromStart = 0;
    if (m_points.size() > 0) {
        const Point& last = m_points.back();
        distanceFromStart = last.distanceFromStart + distance(last.point, a);
    }

    m_points.push_back({ a, distanceFromStart });

    // should use ring buffer
    if (m_points.size() >= 100) {
        m_points.erase(m_points.begin());

        float baselineLength = m_points.front().distanceFromStart;
        for (Point& p : m_points) {
            p.distanceFromStart -= baselineLength;
        }
    }
}

float LineMesh::totalLength() const {
    if (m_points.size() == 0) {
        return 0.f;
    }

    return m_points.back().distanceFromStart; 
}

std::vector<LineMesh::Point>& LineMesh::points() {
    return m_points;
}

VertexLayout LineMesh::getLayout() {
    VertexLayout vertexLayout = {
        .buffers = {
            {
                .binding = 0,
                .stride = sizeof(Vertex),
                .instanced = false,
                .attributes = {
                    {
                        .location = 0,
                        .offset = offsetof(Vertex, pos),
                        .format = AttributeFormat_Float3
                    }                    
                }
            },
            {
                .binding = 1,
                .stride = sizeof(Point),
                .instanced = true,
                .attributes = {
                    {
                        .location = 1,
                        .offset = offsetof(Point, point),
                        .format = AttributeFormat_Float2
                    },
                    {
                        .location = 2,
                        .offset = offsetof(Point, distanceFromStart),
                        .format = AttributeFormat_Float
                    },
                    {
                        .location = 3,
                        .offset = sizeof(Point) + offsetof(Point, point),
                        .format = AttributeFormat_Float2
                    },
                    {
                        .location = 4,
                        .offset = sizeof(Point) + offsetof(Point, distanceFromStart),
                        .format = AttributeFormat_Float
                    }
                }
            }
        }
    };

    return vertexLayout;
}