#pragma once
#include <array>
#include <glm/glm.hpp>
#include "OpenGL_support/VAO.hpp"

constexpr int CHUNK_SIDE = 16;

class World;

struct Block
{
    unsigned char type;
    Block(const unsigned char& tp) : type(tp){};
    Block(): type(0){};
};

//TODO: add structure for holding side data with block
// 1 byte to store which sides are visible
// when generating vertices we can use this data instead of
//needing to recalculate


// RENDERING IDEAS:
// - use a buffer on the gpu, everytime the chunk is updated we update the buffer (side data stored in block)
//      -> every change requires to redo the entire buffer
//      -> 0 cpu memory required
// - we store an array of all the sides needed to be drawn in an array (pos, direction), and pass this array to a batch renderer every frame
//      when there is a change, we simply add/remove an entry into this buffer
//      -> easy to change
//          HOW CAN WE REMOVE A SPECIFIC FACE?
//      -> dynamic VBO
//      -> requires additional cpu memory
//          -> we can store this entire buffer on the renderer and not at the chunk level
//side note:
//  max number of vertices/chunk = CHUNK_SIDE*CHUNK_SIDE*CHUNK_SIDE * 6 * 4 / 2 since two blocks cant render the same side 

class Chunk
{

public:
    Chunk(World& world);
    Chunk(int X, int Y, int Z, World& world);
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

    //so world can generate vertices as well
    friend World;

    void generate_vertices();
    const Block* get_block(const glm::vec3& pos) const;
    void set_block(const glm::vec3& pos, const Block& block);


    std::array<Block, CHUNK_SIDE*CHUNK_SIDE*CHUNK_SIDE> m_blocks;
    //should this be here or in world?
    //does hunk generate its vertices or does the world generate vertices
    //for all chunks at once
    VAO vertex_array;
    int vertex_count;

    World& m_world;

};