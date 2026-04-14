#pragma once
#include <glm/glm.hpp>
#include "Defines.hpp"

struct FaceData;
struct VertexData
{
    glm::vec3 pos;
    glm::vec2 texture_pos;
    float texture_id;
    float normal_direction; // corresponds to DIRECTION of the notmal
    VertexData(const glm::vec3& p, const glm::vec2& tp, const unsigned int& id, const float& normal)
    :pos(p), texture_pos(tp), texture_id(id), normal_direction(normal) {}
    VertexData() : VertexData({0, 0, 0}, {0, 0}, 0U, 0) {}
};

struct RenderData
{
    unsigned int VAO;
    unsigned int index_count;

    RenderData(unsigned int v, unsigned int c)
    : VAO(v), index_count(c) {}
};

//TODO: memory management
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

    //draw filled block
    void draw_block(const glm::vec3& pos, const glm::vec3& color);


private:

    void add_quad(const glm::vec3& block_pos, DIRECTION dir);

    //used for dynamic rendering
    unsigned int m_VAO;
    unsigned int m_VBO;

    unsigned int indices_to_draw;

    RenderData m_static_geometry;

    VertexData* bufferPtr;
    VertexData* currentPtr;
    unsigned int* indices;

    int draw_calls;

};