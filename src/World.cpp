#include "World.hpp"

#include <iostream>
#include "GLFW/glfw3.h"
#include "Renderer.hpp"

World::World(Renderer& r)
{
    m_loaded_chunks.reserve(8*RENDER_DISTANCE*RENDER_DISTANCE*RENDER_DISTANCE);

    for(int i = -RENDER_DISTANCE; i < RENDER_DISTANCE+1; i++)
    {
        for(int j = -RENDER_DISTANCE; j < RENDER_DISTANCE+1; j++)
        {
            for(int k = -RENDER_DISTANCE; k < RENDER_DISTANCE+1; k++)
            {
                m_loaded_chunks.emplace_back(j, k, i, *this);
            }
        }
    }

    for(auto& c : m_loaded_chunks)
    {
        c.generate_faces();
    }

    m_loaded_chunks.at(0).set_face({8, 8, 8}, DOWN, true);

    //we need to generate vertices after all chunks have their blocks
    //so we can check edges
    unsigned int current = 0;
    for(auto& c : m_loaded_chunks)
    {
        current = c.generate_face_vertices(m_vertices, current);
    }

    m_vertices.shrink_to_fit();

    std::cout << "Size of single block: " << sizeof(Block) << "\n";
    std::cout << "Size of chunk: " << sizeof(Chunk) << "\n";
    std::cout << "Size of World: " << sizeof(World) 
    << "\nNumber, Size of chunks: " << m_loaded_chunks.size() << ", " 
    << m_loaded_chunks.size()*sizeof(Chunk) << "\n";

    r.add_static_geometry(m_vertices.size(), m_vertices.data());
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
}

void World::render(Renderer& renderer)
{
    double start = glfwGetTime();
   
    renderer.render_static_geometry();
    
    double end = glfwGetTime();
    double deltaTime = end-start;
}

const Chunk* World::get_chunk(const glm::vec3& pos) const
{
    auto index = std::find_if(m_loaded_chunks.begin(), m_loaded_chunks.end(), [&pos](const Chunk& c){return c.pos == pos;});
    
    if(index == m_loaded_chunks.end())
    {
        return nullptr;
    }

    return &*index;
}

const Block* World::get_block(const glm::vec3 &pos) const
{

    int chunk_x = floor(pos.x/CHUNK_SIDE);
    int chunk_y = floor(pos.y/CHUNK_SIDE);
    int chunk_z = floor(pos.z/CHUNK_SIDE);

    glm::vec3 chunk_pos(pos/(float)CHUNK_SIDE);

    const Chunk* chunk = get_chunk(chunk_pos);

    if(!chunk) return nullptr;

    int block_chunk_x = pos.x - chunk_x*CHUNK_SIDE;
    int block_chunk_y = pos.y - chunk_y*CHUNK_SIDE;
    int block_chunk_z = pos.z - chunk_z*CHUNK_SIDE;

    return chunk->get_block(glm::vec3(block_chunk_x, block_chunk_y, block_chunk_z));
}
