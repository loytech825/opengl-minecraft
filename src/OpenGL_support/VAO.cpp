#include "OpenGL_support/VAO.hpp"
#include "glad/glad.h"

VAO::VAO()
: m_ID(0)
{
    glGenVertexArrays(1, &m_ID);
}

VAO::VAO(VAO&& other)
{
    this->m_ID = other.m_ID;
    other.m_ID = 0;
}

VAO& VAO::operator=(VAO&& other)
{
    this->m_ID = other.m_ID;
    other.m_ID = 0;
    return *this;
}

VAO::~VAO()
{
    if(m_ID) glDeleteVertexArrays(1, &m_ID);
}

void VAO::bind() const
{
    glBindVertexArray(m_ID);
}

void VAO::unbind() const
{
    glBindVertexArray(0);
}

void VAO::add_buffer(const VBO& buffer, const VertexBufferLayout& layout)
{
    //FIXME: NON URGENT: VAO can have multiple buffers, indexes continue  
    bind();
    buffer.bind();
    const auto& elements = layout.get_elements();
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);


    unsigned int offset = 0;
    for(int i = 0; i < elements.size(); i++)
    {
        const auto& el = elements.at(i);
        glVertexAttribPointer(i, el.count, el.type, el.normalized, layout.get_stride(), (void*)offset);
        glEnableVertexAttribArray(i);
        offset += sizeof(el.type);
    }

    buffer.unbind();
    unbind();
}