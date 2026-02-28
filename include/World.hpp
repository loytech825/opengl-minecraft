#pragma once
#include "Chunk.hpp"

#include <vector>

constexpr int RENDER_DISTANCE = 4;


class World
{
public:
    World();
    void render();

    //returns a raw pointer to a chunk at position
    //returns null if chunk not loaded
    const Chunk* get_chunk(const glm::vec3& pos) const;

private:
    std::vector<Chunk> loadedChunks;
};