#pragma once
#include <vector>
#include "glad/glad.h"

struct VertexArrayElement
{
    //number of data "numbers"
    unsigned int count;
    //GL type
    GLenum type;
    GLboolean normalized;

    //size of elements in bytes
    //TODO: function from GLenum type -> size or extra field
    unsigned int size;

    VertexArrayElement(int c, GLenum t, GLboolean n, unsigned int s)
    : count(c), type(t), normalized(n), size(s){}
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