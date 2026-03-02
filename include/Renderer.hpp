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

class Renderer
{

public:
    Renderer();
    ~Renderer();
    void add_face(const FaceData& face);
    void add_vertex(const VertexData& v);
    void add_vertices(unsigned int count, const VertexData* data);

    void init_batch();
    void flush();


private:
    unsigned int m_VAO;
    unsigned int m_VBO;

    unsigned int indices_to_draw;

    VertexData* bufferPtr;
    VertexData* currentPtr;
    unsigned int* indices;

};