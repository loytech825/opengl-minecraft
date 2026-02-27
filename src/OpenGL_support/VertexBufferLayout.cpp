#include "OpenGL_support/VertexBufferLayout.hpp"
#include <iostream>

VertexBufferLayout::VertexBufferLayout()
: m_stride(0)
{}

template<typename T>
void VertexBufferLayout::push(unsigned int count, bool normalized)
{
    std::cerr << "No function overload for type!\n";
    static_assert(false);
}

template<>
void VertexBufferLayout::push<float>(unsigned int count, bool normalized)
{
    m_elements.emplace_back(count, GL_FLOAT, normalized, sizeof(float)*count);
    m_stride += sizeof(float)*count;
}

template<>
void VertexBufferLayout::push<int>(unsigned int count, bool normalized)
{
   m_elements.emplace_back(count, GL_INT, normalized, sizeof(int)*count); 
   m_stride += sizeof(int)*count;
}

template<>
void VertexBufferLayout::push<unsigned int>(unsigned int count, bool normalized)
{
   m_elements.emplace_back(count, GL_UNSIGNED_INT, normalized, sizeof(unsigned int)*count);
   m_stride += sizeof(unsigned int)*count;
}

template<>
void VertexBufferLayout::push<unsigned char>(unsigned int count, bool normalized)
{
   m_elements.emplace_back(count, GL_UNSIGNED_BYTE, normalized, sizeof(unsigned char)*count);
   m_stride += sizeof(unsigned char)*count;
}

const std::vector<VertexArrayElement> &VertexBufferLayout::get_elements() const
{
    return m_elements;
}

unsigned int VertexBufferLayout::get_stride() const
{
    return m_stride;
}
