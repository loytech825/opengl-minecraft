#include "Renderer.hpp"
#include "Chunk.hpp"
#include <memory>
#include <cstring>

#include <iostream>
#include <GLFW/glfw3.h>

constexpr unsigned int MAX_FACES = 130000;
constexpr unsigned int MAX_VERTICES = MAX_FACES*4;
constexpr unsigned int MAX_INDICES = MAX_FACES*6;

Renderer::Renderer()
: m_static_geometry(0, 0)
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES*sizeof(VertexData), nullptr, GL_DYNAMIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)offsetof(VertexData, pos));
    glEnableVertexAttribArray(0);

    //since we're drawing quads, indices are always in the same order
    unsigned int* indices = new unsigned int[MAX_INDICES];

    int offset = 0;
    for(int i = 0; i < MAX_INDICES; i+=6)
    {
        indices[i] = 0 + offset;
        indices[i+1] = 1 + offset;
        indices[i+2] = 2 + offset;
        indices[i+3] = 2 + offset;
        indices[i+4] = 3 + offset;
        indices[i+5] = 0 + offset;
        offset += 4;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES*sizeof(unsigned int), indices, GL_STATIC_DRAW);

    delete[] indices;
    glBindVertexArray(0);
    glDeleteBuffers(1, &EBO);

    bufferPtr = new VertexData[MAX_VERTICES];
    currentPtr = bufferPtr;
    indices_to_draw = 0;
}

void Renderer::init_batch()
{
    currentPtr = bufferPtr;
    indices_to_draw = 0;
}

void Renderer::print_draw_calls() {std::cout << "Draw calls: " << draw_calls << "\n";}

void Renderer::set_static_geometry(unsigned int count, VertexData* data)
{

    if(m_static_geometry.VAO) glDeleteVertexArrays(1, &m_static_geometry.VAO);

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(VertexData), data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)offsetof(VertexData, pos));
    glEnableVertexAttribArray(0);

    unsigned int index_count = (count/4)*6;

    unsigned int* indices = new unsigned int[index_count];

    int offset = 0;
    for(int i = 0; i < index_count; i+=6)
    {
        indices[i] = 0 + offset;
        indices[i+1] = 1 + offset;
        indices[i+2] = 2 + offset;
        indices[i+3] = 2 + offset;
        indices[i+4] = 3 + offset;
        indices[i+5] = 0 + offset;
        offset += 4;
    }

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count*sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    m_static_geometry = RenderData(VAO, index_count);
}

void Renderer::render_static_geometry()
{
    glBindVertexArray(m_static_geometry.VAO);
    glDrawElements(GL_TRIANGLES, m_static_geometry.index_count, GL_UNSIGNED_INT, 0);
}

void Renderer::add_vertex(const VertexData& v)
{
    if(currentPtr-bufferPtr == MAX_VERTICES)
    {
        flush();
        init_batch();
    }

    currentPtr->pos = v.pos;
    currentPtr++;
}

void Renderer::add_vertices(unsigned int count, const VertexData* data)
{
    if(currentPtr+count-bufferPtr > MAX_VERTICES)
    {
        unsigned int remaining_space = (MAX_VERTICES - (currentPtr-bufferPtr));
        memcpy(currentPtr, data, remaining_space*sizeof(VertexData));
        indices_to_draw += (remaining_space*6)/4;
        flush();

        init_batch();
        add_vertices((count-remaining_space), data+remaining_space);
        return;
    }

    memcpy(currentPtr, data, count*sizeof(VertexData));
    currentPtr += count;
    indices_to_draw += (count*6)/4;
}

void Renderer::flush()
{ 
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_VERTICES*sizeof(VertexData), bufferPtr);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, indices_to_draw, GL_UNSIGNED_INT, 0);
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_VAO);
    if(bufferPtr)
        delete[] bufferPtr;
}