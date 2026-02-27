#pragma once
#include <array>
#include <glm/glm.hpp>
#include "OpenGL_support/VAO.hpp"

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
    void render();

    glm::vec3 pos;

private:
    std::array<Block, CHUNK_SIDE*CHUNK_SIDE*CHUNK_SIDE> blocks;


    //should this be here or in world?
    //does hunk generate its vertices or does the world generate vertices
    //for all chunks at once
    VAO vertex_array;
    void generate_vertices();
    int vertex_count;

};