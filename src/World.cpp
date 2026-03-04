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

    set_block({8, 0, 8}, Block(1));

    //we need to generate vertices after all chunks have their blocks
    //so we can check edges
    unsigned int current = 0;
    for(auto& c : m_loaded_chunks)
    {
        current = c.generate_face_vertices(m_vertices, current);
        std::cout << c.pos.x << ", " << c.pos.y << ", " << c.pos.z << "\n";
    }

    m_vertices.shrink_to_fit();

    std::cout << "Size of single block: " << sizeof(Block) << "\n";
    std::cout << "Size of chunk: " << sizeof(Chunk) << "\n";
    std::cout << "Size of World: " << sizeof(World) 
    << "\nNumber, Size of chunks: " << m_loaded_chunks.size() << ", " 
    << m_loaded_chunks.size()*sizeof(Chunk) << "\n";

    std::cout << "Vertex count: " << m_vertices.size() << "\n";

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
    auto index = std::find_if(m_loaded_chunks.begin(), m_loaded_chunks.end(), 
                                [&pos](const Chunk& c){return c.pos == pos;});
    
    if(index == m_loaded_chunks.end()) return nullptr;

    return &*index;
}

void World::set_block(const glm::vec3& pos, const Block& block)
{
    Block* main_block = (Block*) get_block(pos);
    if(!main_block) return;


    main_block->type = block.type;

    for(int i = 0; i < DIRECTION::SIZE; i++)
    {
        Block* side_block = (Block*) get_block(pos+direction_vectors[i]);
        std::cout << pos.x << "\t" << (pos+direction_vectors[i]).x << ": " << side_block << "\n";

        if(!side_block) 
        { 
            main_block->sides = (block.sides | (1<<i));
            continue;
        }

        // two cases: block we set is transparent
        main_block->sides |= (1<<i);
        if(main_block->type == 0)
        {
            side_block->sides |= (1<<(DIRECTION::SIZE-1-i));
        }else
        {
            side_block->sides &= ~(1<<(DIRECTION::SIZE-1-i));
        }
    }
}

const Block* World::get_block(const glm::vec3 &pos) const
{
    int chunk_x = floor(pos.x/CHUNK_SIDE);
    int chunk_y = floor(pos.y/CHUNK_SIDE);
    int chunk_z = floor(pos.z/CHUNK_SIDE);

    glm::vec3 chunk_pos(floor(pos/(float)CHUNK_SIDE));

    const Chunk* chunk = get_chunk(chunk_pos);

    if(!chunk) return nullptr;

    int block_chunk_x = pos.x - chunk_pos.x*CHUNK_SIDE;
    int block_chunk_y = pos.y - chunk_pos.y*CHUNK_SIDE;
    int block_chunk_z = pos.z - chunk_pos.z*CHUNK_SIDE;

    return chunk->get_block(glm::vec3(block_chunk_x, block_chunk_y, block_chunk_z));
}
