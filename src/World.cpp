#include "World.hpp"

#include <iostream>

World::World()
{
    loadedChunks.reserve(RENDER_DISTANCE);

    for(int i = -RENDER_DISTANCE; i < RENDER_DISTANCE+1; i++)
    {
        for(int j = -RENDER_DISTANCE; j < RENDER_DISTANCE+1; j++)
        {
            loadedChunks.emplace_back(j, 0, i, *this);
        }
    }

    //we need to generate vertices after all chunks have their blocks
    //so we can check edges
    for(auto& c : loadedChunks)
    {
        c.generate_vertices();
    }

    std::cout << "Size of single block: " << sizeof(Block) << "\n";
    std::cout << "Size of chunk: " << sizeof(Chunk) << "\n";
    std::cout << "Size of World: " << sizeof(World) 
    << "\nNumber, Size of chunks: " << loadedChunks.size() << ", " 
    << loadedChunks.size()*sizeof(Chunk) << "\n";
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