#include "World.hpp"

#include <iostream>
#include "GLFW/glfw3.h"

World::World()
{

    double start = glfwGetTime();
    loadedChunks.reserve(8*RENDER_DISTANCE*RENDER_DISTANCE*RENDER_DISTANCE);

    for(int i = -RENDER_DISTANCE; i < RENDER_DISTANCE+1; i++)
    {
        for(int j = -RENDER_DISTANCE; j < RENDER_DISTANCE+1; j++)
        {
            for(int k = -RENDER_DISTANCE; k < RENDER_DISTANCE+1; k++)
            {
                loadedChunks.emplace_back(j, k, i, *this);
            }
        }
    }

    //we need to generate vertices after all chunks have their blocks
    //so we can check edges
    for(auto& c : loadedChunks)
    {
        c.generate_vertices();
    }

    double end = glfwGetTime();

    std::cout << "Size of single block: " << sizeof(Block) << "\n";
    std::cout << "Size of chunk: " << sizeof(Chunk) << "\n";
    std::cout << "Size of World: " << sizeof(World) 
    << "\nNumber, Size of chunks: " << loadedChunks.size() << ", " 
    << loadedChunks.size()*sizeof(Chunk) << "\n";

    std::cout << "Generation time: " << end-start << "\tAverage per chunk: " << (end-start)/loadedChunks.size() << "\n";
}

void World::render()
{
    for(auto& c: loadedChunks)
    {
        c.render();
    }
}

const Chunk* World::get_chunk(const glm::vec3& pos) const
{
    auto index = std::find_if(loadedChunks.begin(), loadedChunks.end(), [&pos](const Chunk& c){return c.pos == pos;});
    
    if(index == loadedChunks.end())
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
