#pragma once
#include <array>
#include <glm/glm.hpp>
#include "OpenGL_support/VAO.hpp"
#include "Renderer.hpp"

constexpr int CHUNK_SIDE = 16;

class World;
class Renderer;

extern glm::vec3 direction_vectors[];

enum BlockType : unsigned char
{
    AIR = 0, STONE
};

struct Block
{
    BlockType type;
    unsigned char sides;
    Block(const unsigned char& tp) : Block(tp, 0){};
    Block(const unsigned char& tp, const unsigned char s) : type(AIR), sides(s){};
    Block(): type(AIR){};
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
//(face based rendering world storage)
// - we still store an array of all vertices but this time inside the world
//    so we dont have t loop through chunks
//  - each chunk gets a start and an end index for its vertices
//       so we can easily delete it or find a side for a chunk
//  - if a face needs to be deleted we remove 4 vertices from an array
//  - if a face needs to be added its a hell hole
//      (figure out where and how many vert are generated and move 
//       all subsequent ones back)
class Chunk
{

public:
    Chunk(World& world);
    Chunk(int X, int Y, int Z, World& world);

    glm::vec3 pos;

private:

    //so world can generate vertices as well
    friend World;

    void generate_faces();
    unsigned int generate_face_vertices(std::vector<VertexData>& array, unsigned int start_index);

    void generate_block_faces(const glm::vec3& pos, const BlockType type);
    //Sets block at pos and updates its faces and surrounding block faces
    const Block* get_block(const glm::vec3& pos) const;
    void set_block(const glm::vec3& pos, const Block& block);

    /*
    @brief Sets face of block to value
    @param pos block position in chunk
    @param dir direction of face to se
    @param value to set the face to
    */
    void set_face(const glm::vec3& pos, DIRECTION dir, bool value);


    std::array<Block, CHUNK_SIDE*CHUNK_SIDE*CHUNK_SIDE> m_blocks;
    

    //metadata
    //index of where in the array this chunk's vertex data starts
    unsigned int vertex_start;
    //how many vertices this chunk has
    unsigned int vertex_size;
    unsigned int vertex_end;
    //indicates if the vertices need to be regenerated
    bool dirty;

    World& m_world;

};