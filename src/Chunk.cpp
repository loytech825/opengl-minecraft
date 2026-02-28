#include "Chunk.hpp"
#include "World.hpp"

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
    //down
    glm::vec3(0, -1, 0),
    //north
    glm::vec3(1, 0, 0),
    //south
    glm::vec3(-1, 0, 0),
    //east
    glm::vec3(0, 0, 1),
    //west
    glm::vec3(0, 0, -1)
};

enum DIRECTION : unsigned char
{
    UP = 0, DOWN, NORTH, SOUTH, EAST, WEST
};

//generates the vertex position for a single side of a block
//at a position
const std::vector<float> generate_side_vertices(DIRECTION dir, const glm::vec3& block_pos)
{
    std::vector<float> vertices;
    vertices.reserve(18);

    for(int i = 0; i <18; i+=3)
    {
        vertices.push_back(cube_vertices[dir*18+i] + block_pos.x);
        vertices.push_back(cube_vertices[dir*18+i+1] + block_pos.y);
        vertices.push_back(cube_vertices[dir*18+i+2] + block_pos.z);
    }

    return vertices;
}

Chunk::Chunk(int X, int Y, int Z, World& world)
:   pos(X, Y, Z),
    m_world(world)
{
    std::fill(blocks.begin(), blocks.end(), (unsigned char)1);
}

Chunk::Chunk(World& world) : Chunk(0, 0, 0, world) {}

//DEBUG
void Chunk::print(){

    for(const auto& blk : blocks){
        std::cout << (unsigned short)blk.type << "\n";
    }
}

void Chunk::render()
{
    vertex_array.bind();
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    vertex_array.unbind();
}


//This can be slow, we need to regen the entire buffer if a single block changes
//TODO: maybe add vertex generation to gpu (compute shader)
//5ms / 16²
//66ms / 64²
void Chunk::generate_vertices()
{

    double start = glfwGetTime();

    std::vector<float> v;
    v.reserve(CHUNK_SIDE*CHUNK_SIDE*CHUNK_SIDE*2*4*3);

    for(int y = 0; y < CHUNK_SIDE; y++)
    {
        for(int z = 0; z < CHUNK_SIDE; z++)
        {
            for(int x = 0; x < CHUNK_SIDE; x++)
            {
                //if there is no block here skip
                if(!blocks[y*CHUNK_SIDE*CHUNK_SIDE+z*CHUNK_SIDE+x].type) continue;

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
                        if(blocks[index].type) continue;

                    }
                    //Block not in this chunk!
                    
                    auto other_pos = pos + glm::vec3(rel_x, rel_y, rel_z);
                    const Chunk* other = m_world.get_chunk(other_pos);
  
                    if(other)
                    {
                        //std::cout << "Pos: " << pos.x << ", " << pos.y << ", " << pos.z << "\tRel: "
                        //<< rel_x << ", " << rel_y << ", " << rel_z << "\tOther: " 
                        //<< other_pos.x << ", " << other_pos.y << ", " << other_pos.z 
                        //<< "\t" << other << "\n";
                        continue;
                    }
                    

                    auto face_vertices = generate_side_vertices((DIRECTION)i, glm::vec3(x, y, z)+(float)CHUNK_SIDE*pos);
                    v.insert(v.end(), face_vertices.begin(), face_vertices.end());
                }
            }
        }
    }

    vertex_count = v.size()/3;

    VBO vertices(GL_ARRAY_BUFFER, GL_STATIC_DRAW, v.data(), v.size()*sizeof(float));

    VertexBufferLayout layout;
    layout.push<float>(3, false);

    vertex_array.set_buffer(vertices, layout);

    double end = glfwGetTime();

    std::cout << "Vertex generation time: " << end-start << "\n";
}
