#pragma once
#include "Chunk.hpp"

#include <vector>

constexpr int RENDER_DISTANCE = 4;


class World
{
public:
    World();

private:
    std::vector<Chunk> loadedChunks;
};