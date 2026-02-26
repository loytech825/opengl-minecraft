#pragma once
#include <vector>
#include "glad/glad.h"

struct VertexArrayElement
{
    unsigned int count;
    GLenum type;
    GLboolean normalized;

    VertexArrayElement(int c, GLenum t, GLboolean n)
    : count(c), type(t), normalized(n){}
};

class VertexBufferLayout
{

/*
                    INDEX  SIZE     TYPE       NORMALIZED   STRIDE             BEGINNING INDEX
glVertexAttribPointer(0,    3,    GL_FLOAT,   GL_FALSE,     3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
*/
public:
    VertexBufferLayout();
    template<typename T>
    void push(unsigned int count, bool normalized);
    const std::vector<VertexArrayElement>& get_elements() const;
    unsigned int get_stride() const;


private:
    std::vector<VertexArrayElement> m_elements;
    unsigned int m_stride;
};