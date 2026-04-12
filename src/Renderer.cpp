#include "Renderer.hpp"
#include "Chunk.hpp"
#include <memory>
#include <cstring>

#include <iostream>
#include <GLFW/glfw3.h>

constexpr unsigned int MAX_FACES = 4000;
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


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, texture_pos));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, texture_id));
    glEnableVertexAttribArray(2);

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

    //vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, texture_pos));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, texture_id));
    glEnableVertexAttribArray(2);

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

void Renderer::draw_block(const glm::vec3& pos, const glm::vec3& color)
{
    add_quad(pos, UP);
    add_quad(pos, DOWN);
    add_quad(pos, EAST);
    add_quad(pos, WEST);
    add_quad(pos, NORTH);
    add_quad(pos, SOUTH);
}

void Renderer::add_quad(const glm::vec3& block_pos, DIRECTION dir)
{
    //slightly larger so it renders over regular blocks
    constexpr float offset = 0.01;
    constexpr float zero = -offset;
    constexpr float one = 1+offset;

    float X = zero, Y = zero, Z = zero;

    switch(dir)
    {
        case UP:
            Y = one;
        case DOWN:
            add_vertex(VertexData(block_pos+glm::vec3(zero, Y, zero), glm::vec2(0, 0), 0));
            add_vertex(VertexData(block_pos+glm::vec3(zero, Y, one), glm::vec2(0, 1), 0));
            add_vertex(VertexData(block_pos+glm::vec3(one, Y, one), glm::vec2(1, 1), 0));
            add_vertex(VertexData(block_pos+glm::vec3(one, Y, zero), glm::vec2(1, 0), 0));
            break;

        case NORTH:
            X = one;
        case SOUTH:
            add_vertex(VertexData(block_pos+glm::vec3(X, zero, zero), glm::vec2(0, 0), 0));
            add_vertex(VertexData(block_pos+glm::vec3(X, zero, one), glm::vec2(0, 1), 0));
            add_vertex(VertexData(block_pos+glm::vec3(X, one, one), glm::vec2(1, 1), 0));
            add_vertex(VertexData(block_pos+glm::vec3(X, one, zero), glm::vec2(1, 0), 0));
            break;
        
        case EAST:
            Z = one;
        case WEST:
            add_vertex(VertexData(block_pos+glm::vec3(zero, zero, Z), glm::vec2(0, 0), 0));
            add_vertex(VertexData(block_pos+glm::vec3(zero, one, Z), glm::vec2(0, 1), 0));
            add_vertex(VertexData(block_pos+glm::vec3(one, one, Z), glm::vec2(1, 1), 0));
            add_vertex(VertexData(block_pos+glm::vec3(one, zero, Z), glm::vec2(1, 0), 0));
            break;
    }

    indices_to_draw += 6;
}

void Renderer::add_vertex(const VertexData& v)
{
    if(currentPtr-bufferPtr == MAX_VERTICES)
    {
        flush();
        init_batch();
    }

    currentPtr->pos = v.pos;
    currentPtr->texture_id = v.texture_id;
    currentPtr->texture_pos = v.texture_pos;
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

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE_STRIP);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_VERTICES*sizeof(VertexData), bufferPtr);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, indices_to_draw, GL_UNSIGNED_INT, 0);
    std::cout << indices_to_draw << "i\n";

    //glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_VAO);
    if(bufferPtr)
        delete[] bufferPtr;
}