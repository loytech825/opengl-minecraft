#include "Renderer.hpp"
#include "Chunk.hpp"
#include <memory>
#include <cstring>

constexpr unsigned int MAX_FACES = 10000;
constexpr unsigned int MAX_VERTICES = MAX_FACES*4;
constexpr unsigned int MAX_INDICES = MAX_FACES*6;

Renderer::Renderer()
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
}

void Renderer::init_batch()
{
    if(bufferPtr != nullptr)
        flush();

    bufferPtr = new VertexData[MAX_VERTICES];
    currentPtr = bufferPtr;
    indices_to_draw = 0;
}

void Renderer::add_face(const FaceData& face)
{
    VertexData v;

    //bottom south west = origin
    switch(face.direction)
    {
        case UP:
            v.pos = glm::vec3(face.x, face.y+1, face.z);
            add_vertex(v);
            v.pos = glm::vec3(face.x, face.y+1, face.z+1);
            add_vertex(v);
            v.pos = glm::vec3(face.x+1, face.y+1, face.z+1);
            add_vertex(v);
            v.pos = glm::vec3(face.x+1, face.y+1, face.z);
            add_vertex(v);
            break;
        
        case DOWN:
            v.pos = glm::vec3(face.x, face.y, face.z);
            add_vertex(v);
            v.pos = glm::vec3(face.x, face.y, face.z+1);
            add_vertex(v);
            v.pos = glm::vec3(face.x+1, face.y, face.z+1);
            add_vertex(v);
            v.pos = glm::vec3(face.x+1, face.y, face.z);
            add_vertex(v);
            break;
        
        case NORTH:
            v.pos = glm::vec3(face.x+1, face.y, face.z);
            add_vertex(v);
            v.pos = glm::vec3(face.x+1, face.y, face.z+1);
            add_vertex(v);
            v.pos = glm::vec3(face.x+1, face.y+1, face.z+1);
            add_vertex(v);
            v.pos = glm::vec3(face.x+1, face.y+1, face.z);
            add_vertex(v);
            break;

        case SOUTH:
            v.pos = glm::vec3(face.x, face.y, face.z);
            add_vertex(v);
            v.pos = glm::vec3(face.x, face.y, face.z+1);
            add_vertex(v);
            v.pos = glm::vec3(face.x, face.y+1, face.z+1);
            add_vertex(v);
            v.pos = glm::vec3(face.x, face.y+1, face.z);
            add_vertex(v);
            break;

        case WEST:
            v.pos = glm::vec3(face.x, face.y, face.z);
            add_vertex(v);
            v.pos = glm::vec3(face.x+1, face.y, face.z);
            add_vertex(v);
            v.pos = glm::vec3(face.x+1, face.y+1, face.z);
            add_vertex(v);
            v.pos = glm::vec3(face.x, face.y+1, face.z);
            add_vertex(v);
            break;
        
        case EAST:
            v.pos = glm::vec3(face.x, face.y, face.z+1);
            add_vertex(v);
            v.pos = glm::vec3(face.x+1, face.y, face.z+1);
            add_vertex(v);
            v.pos = glm::vec3(face.x+1, face.y+1, face.z+1);
            add_vertex(v);
            v.pos = glm::vec3(face.x, face.y+1, face.z+1);
            add_vertex(v);
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
    currentPtr++;
}

void Renderer::add_vertices(unsigned int count, const VertexData* data)
{
    if(currentPtr+count-bufferPtr > MAX_VERTICES)
    {
        //unsigned int remaining_space = (MAX_VERTICES - (currentPtr-bufferPtr));
        //memcpy(currentPtr, data, remaining_space*sizeof(VertexData));
        //add_vertices((count-remaining_space), data+remaining_space);
        flush();
        init_batch();
        //return;
    }

    memcpy(currentPtr, data, count*sizeof(VertexData));
    currentPtr += count;
    indices_to_draw += (count*6)/4;
}

void Renderer::flush()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_VERTICES*sizeof(VertexData), bufferPtr);
    delete[] bufferPtr;
    bufferPtr = nullptr;
    currentPtr = nullptr;

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, indices_to_draw, GL_UNSIGNED_INT, 0);
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_VAO);
    if(bufferPtr)
        delete[] bufferPtr;
}