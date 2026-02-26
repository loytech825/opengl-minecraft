#pragma once
#include <array>
#include "OpenGL_support/VBO.hpp"

constexpr int CHUNK_SIDE = 16;


struct Block
{
    unsigned char type;
    Block(const unsigned char& tp) : type(tp){};
    Block(): type(0){};
};

class Chunk
{

public:
    Chunk();
    Chunk(int X, int Y, int Z);
    // since chunk only contains Blocks (for now)
    // we dont need any special resource management

    /*Chunk(const Chunk& other);
    Chunk& operator=(const Chunk& other);
    Chunk(Chunk&& other);
    Chunk& operator=(Chunk&& other);*/

    //~Chunk();

    //DEBUG
    void print();
    int x, y, z;

private:
    std::array<Block, CHUNK_SIDE*CHUNK_SIDE*CHUNK_SIDE> blocks;
    VBO vertices;
};