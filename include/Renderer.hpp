#pragma once
#include <glm/glm.hpp>

struct FaceData;

struct VertexData
{
    glm::vec3 pos;
    VertexData(const glm::vec3& p)
    :pos(p) {}
    VertexData() : VertexData({0, 0, 0}) {}
};

struct RenderData
{
    unsigned int VAO;
    unsigned int index_count;

    RenderData(unsigned int v, unsigned int c)
    : VAO(v), index_count(c) {}
};

class Renderer
{

public:
    Renderer();
    ~Renderer();
    void add_vertex(const VertexData& v);
    void add_vertices(unsigned int count, const VertexData* data);

    void set_static_geometry(unsigned int count, VertexData* data);
    void render_static_geometry();

    void init_batch();
    void flush();

    void print_draw_calls();
    void reset_draw_calls() {draw_calls=0;}


private:
    unsigned int m_VAO;
    unsigned int m_VBO;

    unsigned int indices_to_draw;

    RenderData m_static_geometry;

    VertexData* bufferPtr;
    VertexData* currentPtr;
    unsigned int* indices;

    int draw_calls;

};