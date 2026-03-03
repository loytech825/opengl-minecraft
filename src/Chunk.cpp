#include "Chunk.hpp"
#include "World.hpp"
#include "Renderer.hpp"

#include <iostream>
#include <math.h>
#include "GLFW/glfw3.h"

//cube vertex position literals
float cube_vertices[] = {

    //top
    0, 1, 0,
    1, 1, 0,
    1, 1, 1,

    1, 1, 1,
    0, 1, 1,
    0, 1, 0,

    //bottom
    0, 0, 0,
    1, 0, 0,
    1, 0, 1,

    1, 0, 1,
    0, 0, 1,
    0, 0, 0,

    //NORTH
    1, 0, 0,
    1, 0, 1,
    1, 1, 1,

    1, 1, 1,
    1, 1, 0,
    1, 0, 0,

    //SOUTH
    0, 0, 0,
    0, 0, 1,
    0, 1, 1,

    0, 1, 1,
    0, 1, 0,
    0, 0, 0,

    //EAST
    0, 0, 1,
    1, 0, 1,
    1, 1, 1,

    1, 1, 1,
    0, 1, 1,
    0, 0, 1,

    //WEST
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,

    1, 1, 0,
    0, 1, 0,
    0, 0, 0,
};

glm::vec3 direction_vectors[] = {
    //up
    glm::vec3(0, 1, 0),
    //north
    glm::vec3(1, 0, 0),
    //east
    glm::vec3(0, 0, 1),
    //west
    glm::vec3(0, 0, -1),
    //south
    glm::vec3(-1, 0, 0),
    //down
    glm::vec3(0, -1, 0),
};

//generates the vertex position for a single side of a block
//at a position
void generate_side_vertices(DIRECTION dir, const glm::vec3& block_pos, std::vector<VertexData>& array)
{

    int X = 0, Y = 0, Z = 0;

    switch(dir)
    {
        case UP:
            Y = 1;
        case DOWN:
            array.emplace_back(block_pos+glm::vec3(0, Y, 0));
            array.emplace_back(block_pos+glm::vec3(0, Y, 1));
            array.emplace_back(block_pos+glm::vec3(1, Y, 1));
            array.emplace_back(block_pos+glm::vec3(1, Y, 0));
            break;

        case NORTH:
            X = 1;
        case SOUTH:
            array.emplace_back(block_pos+glm::vec3(X, 0, 0));
            array.emplace_back(block_pos+glm::vec3(X, 0, 1));
            array.emplace_back(block_pos+glm::vec3(X, 1, 1));
            array.emplace_back(block_pos+glm::vec3(X, 1, 0));
            break;
        
        case EAST:
            Z = 1;
        case WEST:
            array.emplace_back(block_pos+glm::vec3(0, 0, Z));
            array.emplace_back(block_pos+glm::vec3(0, 1, Z));
            array.emplace_back(block_pos+glm::vec3(1, 1, Z));
            array.emplace_back(block_pos+glm::vec3(1, 0, Z));
            break;
    }
}

Chunk::Chunk(int X, int Y, int Z, World& world)
:   pos(X, Y, Z),
    m_world(world)
{
    std::fill(m_blocks.begin(), m_blocks.end(), (unsigned char)1);
    /*set_block({0, 0, 0}, Block(0));
    set_block({1, 0, 0}, Block(0));
    set_block({0, 0, 1}, Block(0));
    set_block({7, 7, 7}, Block(0));*/
}

Chunk::Chunk(World& world) : Chunk(0, 0, 0, world) {}

//DEBUG
void Chunk::print(){

    for(const auto& blk : m_blocks){
        std::cout << (unsigned short)blk.type << "\n";
    }
}

void Chunk::render(Renderer& renderer)
{
}

//This can be slow, we need to regen the entire buffer if a single block changes
//TODO: maybe add vertex generation to gpu (compute shader)
//5ms / 16²
//66ms / 64²
unsigned int Chunk::generate_faces(std::vector<VertexData>& array, unsigned int start_index)
{
    unsigned int offset = 0;
    for(int y = 0; y < CHUNK_SIDE; y++)
    {
        for(int z = 0; z < CHUNK_SIDE; z++)
        {
            for(int x = 0; x < CHUNK_SIDE; x++)
            {
                //if there is no block here skip
                if(!m_blocks[y*CHUNK_SIDE*CHUNK_SIDE+z*CHUNK_SIDE+x].type) continue;

                //check all six sides
                for(int i = 0; i < 6; i++)
                {

                    int index = (direction_vectors[i].y+y)*CHUNK_SIDE*CHUNK_SIDE
                                +(direction_vectors[i].z+z)*CHUNK_SIDE
                                +(direction_vectors[i].x+x);

                    //checks if neighboring block is in chunk
                    //these three ints get us a chunk offset of the neighboring block
                    //all three == 0, when block is in this chunk

                    const int rel_x = floor((float)(x+(int)direction_vectors[i].x)/CHUNK_SIDE);
                    const int rel_y = floor((float)(y+(int)direction_vectors[i].y)/CHUNK_SIDE);
                    const int rel_z = floor((float)(z+(int)direction_vectors[i].z)/CHUNK_SIDE);

                    const bool in_x_limit = rel_x == 0;
                    const bool in_y_limit = rel_y == 0;
                    const bool in_z_limit = rel_z == 0;

                    if(in_x_limit && in_y_limit && in_z_limit)
                    {
                        //checks if neighboring block exists
                        if(m_blocks[index].type)
                        {
                            //std::cout << ": " << x << ", " << y << ", " << z << ": " << m_blocks[index].type << "\n";
                            continue;
                        }
                    }else
                    {
                        //Block not in this chunk!
                        auto other_pos = pos + glm::vec3(rel_x, rel_y, rel_z);
                        const Chunk* other = m_world.get_chunk(other_pos);
    
                        if(other)
                        {
                            glm::vec3 relative_block_pos = glm::vec3(x, y, z) + direction_vectors[i];
                            glm::vec3 absolute_block_pos = relative_block_pos + pos*(float)CHUNK_SIDE;

                            const Block* neighbor = m_world.get_block(absolute_block_pos);

                            if(!neighbor)
                                continue;

                            if(neighbor->type != 0)
                                continue;
                        }
                    }
                    
                    generate_side_vertices((DIRECTION)i, glm::vec3(x, y, z)+pos, array);
                    offset+=4;
                }
            }
        }
    }

   return offset+start_index;
}

const Block* Chunk::get_block(const glm::vec3& pos) const
{

    const bool check_x = pos.x >= 0 && pos.x < CHUNK_SIDE; 
    const bool check_y = pos.y >= 0 && pos.y < CHUNK_SIDE;
    const bool check_z = pos.z >= 0 && pos.z < CHUNK_SIDE;
    
    if(check_x && check_y && check_z)
        return &m_blocks[pos.y*CHUNK_SIDE*CHUNK_SIDE+pos.z*CHUNK_SIDE+pos.x];

    return nullptr;
}

void Chunk::set_block(const glm::vec3& pos, const Block& block)
{
    const bool check_x = pos.x >= 0 && pos.x < CHUNK_SIDE; 
    const bool check_y = pos.y >= 0 && pos.y < CHUNK_SIDE;
    const bool check_z = pos.z >= 0 && pos.z < CHUNK_SIDE;
    
    if(check_x && check_y && check_z)
        m_blocks[pos.y*CHUNK_SIDE*CHUNK_SIDE+pos.z*CHUNK_SIDE+pos.x] = block;
}
