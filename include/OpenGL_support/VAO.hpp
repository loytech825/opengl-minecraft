#pragma once
#include "OpenGL_support/VBO.hpp"
#include "OpenGL_support/VertexBufferLayout.hpp"

class VAO
{
public:
    VAO();
    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;

    VAO(VAO&& other);
    VAO& operator=(VAO&& other);

    ~VAO();

    void add_buffer(const VBO& buffer, const VertexBufferLayout& layout);

    void bind() const;
    void unbind() const;

private:
    unsigned int m_ID;
};