#include "World.hpp"

#include <iostream>

World::World()
{
    loadedChunks.reserve(RENDER_DISTANCE);

    for(int i = -RENDER_DISTANCE; i < RENDER_DISTANCE+1; i++)
    {
        for(int j = -RENDER_DISTANCE; j < RENDER_DISTANCE+1; j++)
        {
            loadedChunks.emplace_back(j, i, 0);
        }
    }

    std::cout << "Size of single block: " << sizeof(Block) << "\n";
    std::cout << "Size of chunk: " << sizeof(Chunk) << "\n";
    std::cout << "Size of World: " << sizeof(World) 
    << "\nNumber, Size of chunks: " << loadedChunks.size() << ", " 
    << loadedChunks.size()*sizeof(Chunk) << "\n";

}