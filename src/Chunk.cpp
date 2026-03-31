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

inline unsigned int pos_to_index(int x, int y, int z)
{
    return y*CHUNK_SIDE*CHUNK_SIDE+z*CHUNK_SIDE+x;
}

inline unsigned int pos_to_index(const glm::vec3& pos)
{
    return pos.y*CHUNK_SIDE*CHUNK_SIDE+pos.z*CHUNK_SIDE+pos.x;
}

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
            array.emplace_back(block_pos+glm::vec3(0, Y, 0), glm::vec2(0, 0));
            array.emplace_back(block_pos+glm::vec3(0, Y, 1), glm::vec2(0, 1));
            array.emplace_back(block_pos+glm::vec3(1, Y, 1), glm::vec2(1, 1));
            array.emplace_back(block_pos+glm::vec3(1, Y, 0), glm::vec2(1, 0));
            break;

        case NORTH:
            X = 1;
        case SOUTH:
            array.emplace_back(block_pos+glm::vec3(X, 0, 0), glm::vec2(0, 0));
            array.emplace_back(block_pos+glm::vec3(X, 0, 1), glm::vec2(0, 1));
            array.emplace_back(block_pos+glm::vec3(X, 1, 1), glm::vec2(1, 1));
            array.emplace_back(block_pos+glm::vec3(X, 1, 0), glm::vec2(1, 0));
            break;
        
        case EAST:
            Z = 1;
        case WEST:
            array.emplace_back(block_pos+glm::vec3(0, 0, Z), glm::vec2(0, 0));
            array.emplace_back(block_pos+glm::vec3(0, 1, Z), glm::vec2(0, 1));
            array.emplace_back(block_pos+glm::vec3(1, 1, Z), glm::vec2(1, 1));
            array.emplace_back(block_pos+glm::vec3(1, 0, Z), glm::vec2(1, 0));
            break;
    }
}

Chunk::Chunk(int X, int Y, int Z, World& world)
:   pos(X, Y, Z),
    m_world(world),
    vertex_start(0),
    vertex_size(0),
    vertex_end(0),
    dirty(true)
{
    //very simple world gen
    if(pos.y > 0)
        std::fill(m_blocks.begin(), m_blocks.end(), AIR);
    else
        std::fill(m_blocks.begin(), m_blocks.end(), STONE);

    //std::fill(m_blocks.begin(), m_blocks.end(), (unsigned char)0);
}

Chunk::Chunk(World& world) : Chunk(0, 0, 0, world) {}

void Chunk::generate_all_faces()
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

        //check if this block is transparent
        //if it is we need to generate neighbor's faces
        bool neighbors_visible = this_block.type == 0;

        //Here i dont use the generate_block_faces function because we preloaded the neighboring chunks
        //so this is faster
    
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
                    neighbor_chunks[i]->set_face(relative_block_pos, (DIRECTION)(DIRECTION::SIZE-1-i), neighbors_visible);

                    Block* neighbor = (Block*) neighbor_chunks[i]->get_block(relative_block_pos);

                    //if neighbor is solid, we dont generate a face here
                    if(neighbor->type != 0)
                        continue;
                }//else continue;
            }
            //std::cout << "set side!\n";
            this_block.sides |= 1<<i;
        }
        }
        }
    }
}

unsigned int Chunk::generate_all_vertices(std::vector<VertexData>& array, unsigned int start_index)
{
    m_vertex_gen_mtx.lock();
    m_blocks_access_mtx.lock();
    dirty = false;
    //std::cout << pos.x << ", " << pos.y << ", " << pos.z << ": vertex gen\n";
    unsigned int offset = 0;
    for(int y = 0; y < CHUNK_SIDE; y++)
    {
    for(int z = 0; z < CHUNK_SIDE; z++)
    {
    for(int x = 0; x < CHUNK_SIDE; x++)
    {
        if(m_blocks[pos_to_index(x, y, z)].type == AIR) continue;

        const unsigned char sides = m_blocks[pos_to_index(x, y, z)].sides;

        //std::cout << "BLOCK HERE!" << "\n";
        //std::cout << std::hex << (int)sides << std::dec << "\n"; 

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
    vertex_start = start_index;
    vertex_end = vertex_start+offset;
    vertex_size = offset;
    m_vertex_gen_mtx.unlock();
    m_blocks_access_mtx.unlock();
    return vertex_end;
}

void Chunk::set_face(const glm::vec3& pos, DIRECTION dir, bool value)
{
    Block& block = m_blocks[pos.y*CHUNK_SIDE*CHUNK_SIDE+pos.z*CHUNK_SIDE+pos.x];
    //set bit at location dir to value
    block.sides = (block.sides & ~(1<<dir) | value << dir );
    dirty = true;
}

//WORKS, was using chunk pos instead of block pos
void Chunk::generate_block_faces(const glm::vec3& block_pos, const BlockType type)
{
    m_blocks_access_mtx.lock();
    Block& this_block = m_blocks[pos_to_index(block_pos)];
    this_block.type = type;

    //check if this block is transparent
    //if it is we need to generate neighbor's faces
    bool neighbors_visible = (type == AIR);
    this_block.sides = 0;
    //m_blocks[pos.y*CHUNK_SIDE*CHUNK_SIDE+pos.z*CHUNK_SIDE+pos.x].sides = 255;

    for(int i = 0; i < 6; i++)
    {

        glm::vec3 neighbor_pos = block_pos + direction_vectors[i];

        int index = (neighbor_pos.y)*CHUNK_SIDE*CHUNK_SIDE
                    +(neighbor_pos.z)*CHUNK_SIDE
                    +(neighbor_pos.x);

        //checks if neighboring block is in chunk
        //these three ints get us a chunk offset of the neighboring block
        //all three == 0, when block is in this chunk
        const int rel_x = floor((float)(block_pos.x+(int)direction_vectors[i].x)/CHUNK_SIDE);
        const int rel_y = floor((float)(block_pos.y+(int)direction_vectors[i].y)/CHUNK_SIDE);
        const int rel_z = floor((float)(block_pos.z+(int)direction_vectors[i].z)/CHUNK_SIDE);

        const bool in_x_limit = rel_x == 0;
        const bool in_y_limit = rel_y == 0;
        const bool in_z_limit = rel_z == 0;

        if(in_x_limit && in_y_limit && in_z_limit)
        {
            //checks if neighboring block exists
            if(m_blocks[index].type != AIR)
            {
                set_face(neighbor_pos, (DIRECTION)(DIRECTION::SIZE-1-i), neighbors_visible);
                continue;
            }
        }else
        {
            std::cout << std::to_string((pos+direction_vectors[i]).x) + ", " + std::to_string((pos+direction_vectors[i]).y) + ", " + std::to_string((pos+direction_vectors[i]).z);
            auto neighbor_chunk = (Chunk*)m_world.get_chunk(pos+direction_vectors[i]);
            if(neighbor_chunk)
            {
                std::cout << "CHUNK: neighbor found!\n";
                //direction vector has a 1 or -1 in the direction of neighboring chunk
                //if it has a 1, the coorinate at that position needs to be 0
                // 1 -> add -15
                // -1 -> add 15 
                glm::vec3 relative_block_pos = block_pos + -15.f*direction_vectors[i];

                neighbor_chunk->set_face(relative_block_pos, (DIRECTION)(DIRECTION::SIZE-1-i), neighbors_visible);

                Block* neighbor = (Block*) neighbor_chunk->get_block(relative_block_pos);

                if(neighbor->type != AIR)
                    continue;
                /*std::cout << "Chunk: " << neighbor_chunks[i] 
                            << ", block: " << relative_block_pos.x
                            << ", " << relative_block_pos.y
                            << ", " << relative_block_pos.z << "\n";

                if(!neighbor)
                    continue;

                //Update neighbor block side
                if(this_block.type == 0)
                    neighbor->sides |= 1<<(DIRECTION::SIZE-1-i);
                else
                    neighbor->sides &= ~(1<<(DIRECTION::SIZE-1-i));*/
            }//else continue;
        }
        
        std::cout << i << "\n";
        this_block.sides = this_block.sides | (1<<i); 
        std::cout << std::hex << (int)this_block.sides << std::dec << "\n";
    }
    std::cout << "\n\n\n\n\n";
    m_blocks_access_mtx.unlock();
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

void Chunk::set_block(const glm::vec3& pos, const BlockType type)
{
    const bool check_x = pos.x >= 0 && pos.x < CHUNK_SIDE; 
    const bool check_y = pos.y >= 0 && pos.y < CHUNK_SIDE;
    const bool check_z = pos.z >= 0 && pos.z < CHUNK_SIDE;
    
    if(check_x && check_y && check_z)
    {
        //no need to change anything and no need to regenerate vetices
        if(m_blocks[pos.y*CHUNK_SIDE*CHUNK_SIDE+pos.z*CHUNK_SIDE+pos.x].type == type) return;

        m_blocks[pos.y*CHUNK_SIDE*CHUNK_SIDE+pos.z*CHUNK_SIDE+pos.x].type = type;
        //m_blocks[pos.y*CHUNK_SIDE*CHUNK_SIDE+pos.z*CHUNK_SIDE+pos.x].sides = 255;
        generate_block_faces(pos, type);
        dirty = true;
    }
}
