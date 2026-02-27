#pragma once
#include "OpenGL_support/VertexBufferLayout.hpp"


//TODO: Other buffer types
class VBO
{
public:
    /*
    @brief Creates a new buffer

    @param type any of the OpenGL buffer types
    @param usage expected usage of the data storage
    @param data pointer to beginning of data
    @param size of data in bytes
    */
    VBO(GLenum type, GLenum usage, void* data, unsigned int size);

    //used for initializing buffers whose data is not known at
    //initialization, like class members
    VBO();
    VBO(const VBO& other) = delete;
    VBO& operator=(const VBO& other) = delete;

    VBO(VBO&& other);
    VBO& operator=(VBO&& other);

    ~VBO();

    /*
    @brief Used to set data of an uninitialized buffer

    @param type any of the OpenGL buffer types
    @param usage expected usage of the data storage
    @param data pointer to beginning of data
    @param size of data in bytes
    */
    void set_data(GLenum type, GLenum usage, void* data, unsigned int size);

    void bind() const;
    void unbind() const;

private:
    unsigned int m_ID;
};