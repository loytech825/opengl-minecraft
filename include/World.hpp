#pragma once
#include "Chunk.hpp"

#include <vector>

constexpr int RENDER_DISTANCE = 1;

class Renderer;

class World
{
public:
    World(Renderer& r);
    void render(Renderer& renderer);

    //returns a raw pointer to a chunk at position
    //returns null if chunk not loaded
    const Chunk* get_chunk(const glm::vec3& pos) const;

    /*
    @brief Returns the block at position
    @param pos world position of the block
    @return Pointer to block (nullptr if invalid)
    */
    const Block* get_block(const glm::vec3& pos) const;
    void set_block(const glm::vec3& pos, const Block& block);

private:
    std::vector<Chunk> m_loaded_chunks;
    std::vector<VertexData> m_vertices;
};