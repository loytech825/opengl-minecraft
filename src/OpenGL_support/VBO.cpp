#include "OpenGL_support/VBO.hpp"

VBO::VBO(GLenum type, GLenum usage, void *data, unsigned int size)
{
   set_data(type, usage, data, size);
}

VBO::VBO() 
: m_ID(0)
{

}

void VBO::set_data(GLenum type, GLenum usage, void* data, unsigned int size)
{
    glGenBuffers(1, &m_ID);
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    unbind();
}

VBO::~VBO()
{
    if(m_ID) { glDeleteBuffers(1, &m_ID); }
}

VBO::VBO(VBO &&other)
{
    this->m_ID = other.m_ID;
    other.m_ID = 0;
}

VBO& VBO::operator=(VBO&& other)
{
    this->m_ID = other.m_ID;
    other.m_ID = 0;
    return *this;
}

void VBO::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VBO::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
