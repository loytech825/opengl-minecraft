#include "World.hpp"
#include "Renderer.hpp"

#include <iostream>
#include <math.h>
#include "GLFW/glfw3.h"


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
    //very simple world gen
    if(pos.y > 0)
        std::fill(m_blocks.begin(), m_blocks.end(), (unsigned char)0);
    else
        std::fill(m_blocks.begin(), m_blocks.end(), (unsigned char)1);

    //std::fill(m_blocks.begin(), m_blocks.end(), (unsigned char)0);
}

Chunk::Chunk(World& world) : Chunk(0, 0, 0, world) {}

void Chunk::generate_faces()
{

    std::vector<Chunk*> neighbor_chunks;
    neighbor_chunks.reserve(6);

    for(int i = 0; i < 6; i++)
    {
        neighbor_chunks.push_back((Chunk*)m_world.get_chunk(pos+direction_vectors[i]));
        //std::cout << i << ": Neighbor: " << neighbor_chunks[i] << "\n";
    }

    for(int y = 0; y < CHUNK_SIDE; y++)
    {
        for(int z = 0; z < CHUNK_SIDE; z++)
        {
            for(int x = 0; x < CHUNK_SIDE; x++)
            {
                Block& this_block = m_blocks[y*CHUNK_SIDE*CHUNK_SIDE+z*CHUNK_SIDE+x];

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
                        //we can skip generation for this block if its
                        //empty and in this chunk
                        if(this_block.type == 0)
                            continue;
                        //checks if neighboring block exists
                        if(m_blocks[index].type)
                        {
                            //std::cout << ": " << x << ", " << y << ", " << z << ": " << m_blocks[index].type << "\n";
                            continue;
                        }
                    }else
                    {
                        if(neighbor_chunks[i])
                        {
                            //direction vector has a 1 or -1 in the direction of neighboring chunk
                            //if it has a 1, the coorinate at that position needs to be 0
                            // 1 -> add -15
                            // -1 -> add 15 
                            glm::vec3 relative_block_pos = glm::vec3(x, y, z) + -15.f*direction_vectors[i];

                            Block* neighbor = (Block*) neighbor_chunks[i]->get_block(relative_block_pos);
                            /*std::cout << "Chunk: " << neighbor_chunks[i] 
                                        << ", block: " << relative_block_pos.x
                                        << ", " << relative_block_pos.y
                                        << ", " << relative_block_pos.z << "\n";*/

                            if(!neighbor)
                                continue;

                            //Update neighbor block side
                            if(this_block.type == 0)
                                neighbor->sides |= 1<<(DIRECTION::SIZE-1-i);
                            else
                                neighbor->sides &= ~(1<<(DIRECTION::SIZE-1-i));

                            if(neighbor->type != 0)
                                continue;
                        }//else continue;
                    }
                    m_blocks[y*CHUNK_SIDE*CHUNK_SIDE+z*CHUNK_SIDE+x].sides |= 1<<i;
                }
            }
        }
    }
}

unsigned int Chunk::generate_face_vertices(std::vector<VertexData>& array, unsigned int start_index)
{
    unsigned int offset = 0;
    for(int y = 0; y < CHUNK_SIDE; y++)
    {
    for(int z = 0; z < CHUNK_SIDE; z++)
    {
    for(int x = 0; x < CHUNK_SIDE; x++)
    {
        if(m_blocks[y*CHUNK_SIDE*CHUNK_SIDE+z*CHUNK_SIDE+x].type == 0) continue;

        unsigned char sides = m_blocks[y*CHUNK_SIDE*CHUNK_SIDE+z*CHUNK_SIDE+x].sides;

        if(!sides) continue;
        for(int i = 0; i < 6; i++)
        {
            if(sides & (1<<i))
            {
                generate_side_vertices((DIRECTION)i, glm::vec3(x, y, z)+pos*(float)CHUNK_SIDE, array);
                offset += 4;
            }
        } 
    }
    }
    }
    end = start+offset;
    return end;
}

void Chunk::set_face(const glm::vec3& pos, DIRECTION dir, bool value)
{
    Block& block = m_blocks[pos.y*CHUNK_SIDE*CHUNK_SIDE+pos.z*CHUNK_SIDE+pos.x];
    //set bit at location dir to value
    block.sides = (block.sides & ~(1<<dir) | value << dir );
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
