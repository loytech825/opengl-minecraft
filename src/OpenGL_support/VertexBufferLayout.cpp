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
    m_elements.emplace_back(count, GL_FLOAT, normalized);
    m_stride += sizeof(float);
}

template<>
void VertexBufferLayout::push<int>(unsigned int count, bool normalized)
{
   m_elements.emplace_back(count, GL_INT, normalized); 
   m_stride += sizeof(int);
}

template<>
void VertexBufferLayout::push<unsigned int>(unsigned int count, bool normalized)
{
   m_elements.emplace_back(count, GL_UNSIGNED_INT, normalized);
   m_stride += sizeof(unsigned int);
}

template<>
void VertexBufferLayout::push<unsigned char>(unsigned int count, bool normalized)
{
   m_elements.emplace_back(count, GL_UNSIGNED_BYTE, normalized);
   m_stride += sizeof(unsigned char);
}

const std::vector<VertexArrayElement> &VertexBufferLayout::get_elements() const
{
    return m_elements;
}

unsigned int VertexBufferLayout::get_stride() const
{
    return m_stride;
}
