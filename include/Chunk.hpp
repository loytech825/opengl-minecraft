#pragma once
#include <array>
#include <glm/glm.hpp>
#include "OpenGL_support/VAO.hpp"
#include "Renderer.hpp"

constexpr int CHUNK_SIDE = 16;

class World;
class Renderer;

struct Block
{
    unsigned char type;
    Block(const unsigned char& tp) : type(tp){};
    Block(): type(0){};
};

//configured so opposite side is SIZE-side-1
enum DIRECTION : unsigned char
{
    UP = 0, NORTH, EAST, WEST, SOUTH, DOWN, SIZE
};

struct FaceData
{
    int x, y, z;
    DIRECTION direction;
    FaceData(int X, int Y, int Z, DIRECTION D)
    : x(X), y(Y), z(Z), direction(D){}
};

//TODO: add vertex data here directly instead of face data

// RENDERING IDEAS:
// - use a buffer on the gpu, everytime the chunk is updated we update the buffer (side data stored in block)
//      -> every change requires to redo the entire buffer
//      -> 0 cpu memory required

//(face based batching)
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
    void render(Renderer& renderer);

    glm::vec3 pos;



private:

    //so world can generate vertices as well
    friend World;

    void generate_faces();
    const Block* get_block(const glm::vec3& pos) const;
    void set_block(const glm::vec3& pos, const Block& block);


    std::array<Block, CHUNK_SIDE*CHUNK_SIDE*CHUNK_SIDE> m_blocks;
    std::vector<VertexData> m_vertices;
    //should this be here or in world?
    //does hunk generate its vertices or does the world generate vertices
    //for all chunks at once
    VAO vertex_array;
    int vertex_count;

    World& m_world;

};