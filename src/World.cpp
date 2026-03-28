#include "World.hpp"

#include <iostream>
#include <thread>
#include "GLFW/glfw3.h"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Player.hpp"

World::World(Renderer& r)
:   m_renderer(r),
    m_to_reload(false)
{
    m_loaded_chunks.reserve(8*RENDER_DISTANCE*RENDER_DISTANCE*RENDER_DISTANCE);
    //m_loaded_chunks.reserve(2);

    for(int i = -RENDER_DISTANCE+last_player_chunk_pos.x; i < RENDER_DISTANCE+1+last_player_chunk_pos.x; i++)
    {
        for(int j = -RENDER_DISTANCE + last_player_chunk_pos.y; j < RENDER_DISTANCE+1+last_player_chunk_pos.y; j++)
        {
            for(int k = -RENDER_DISTANCE+last_player_chunk_pos.z; k < RENDER_DISTANCE+1+last_player_chunk_pos.z; k++)
            {
                m_loaded_chunks.emplace_back(i, j, k, *this);
            }
        }
    }

    reload_chunk_faces();
    reload_geometry();

    std::cout << "Size of single block: " << sizeof(Block) << "\n";
    std::cout << "Size of chunk: " << sizeof(Chunk) << "\n";
    std::cout << "Size of World: " << sizeof(World) 
    << "\nNumber, Size of chunks: " << m_loaded_chunks.size() << ", " 
    << m_loaded_chunks.size()*sizeof(Chunk) << "\n";

    std::cout << "Vertex count: " << m_vertices.size() << "\n";

    r.set_static_geometry(m_vertices.size(), m_vertices.data());
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
}

void World::render()
{
    double start = glfwGetTime();
   
    m_renderer.render_static_geometry();
    
    double end = glfwGetTime();
    double deltaTime = end-start;
}

void World::update(const float delta_time, Player& player)
{
    glm::vec3 chunk_player_pos = glm::floor(player.get_position()/(float)CHUNK_SIDE);

    if(chunk_player_pos != last_player_chunk_pos)
    {
        std::cout << "Player entered new chunk!\n";
        last_player_chunk_pos = chunk_player_pos;
        std::thread loading_thread(&World::load_chunks_around_player, this);
        loading_thread.detach();
    }

    if(m_to_reload)
    {
        //loads geometry to gpu buffer
        m_renderer.set_static_geometry(m_vertices.size(), m_vertices.data());
        m_to_reload = false;
    }
}

void World::reload_chunk_faces()
{
    for(auto& c : m_loaded_chunks)
    {
        c.generate_all_faces();
    }
}

void World::reload_geometry()
{
    double start = glfwGetTime();
    auto temp = m_vertices;
    m_vertices.clear();

    //resere a conservative amount of data
    m_vertices.reserve(temp.size()*1.5);
    unsigned int current = 0;
    for(auto& c : m_loaded_chunks)
    {
        if(c.dirty)
        {
            //if chunk changed, we need to reload its vertices
            current = c.generate_all_vertices(m_vertices, current);
        }else
        {
            //data is already inside the temp buffer so we just copy
            std::copy(temp.begin()+c.vertex_start, temp.begin()+c.vertex_end, m_vertices.begin()+current);
            c.vertex_start = current;
            current += c.vertex_size;
        }
    }
    m_vertices.shrink_to_fit();
    m_renderer.set_static_geometry(m_vertices.size(), m_vertices.data());

    double end = glfwGetTime();
    double deltaTime = end-start;

    std::cout << "Vertex generation time new: " << deltaTime << "\n";
}

//works but is probably the slowest shit ever
//made to run on a separate thread
void World::load_chunks_around_player()
{
    //loaded chunks are in range [player_pos - RENDER_DISTANCE, player_pos + RENDER_DISTANCE]
    //check which indices to clear
    std::vector<Chunk*> reassignable_chunks;

    for(int i = 0; i < m_loaded_chunks.size(); i++)
    {
        Chunk& c = m_loaded_chunks[i];
        if(c.pos.x > (last_player_chunk_pos.x + RENDER_DISTANCE) || c.pos.x < (last_player_chunk_pos.x - RENDER_DISTANCE)
        || c.pos.y > (last_player_chunk_pos.y + RENDER_DISTANCE) || c.pos.y < (last_player_chunk_pos.y - RENDER_DISTANCE)
        || c.pos.z > (last_player_chunk_pos.z + RENDER_DISTANCE) || c.pos.z < (last_player_chunk_pos.z - RENDER_DISTANCE))
        {
            reassignable_chunks.push_back(&c);
        }
    }

    //reassignable_chunks should contain all chunks pending for deletion
    int current_index = 0;
    for(int i = -RENDER_DISTANCE+last_player_chunk_pos.x; i < RENDER_DISTANCE+1+last_player_chunk_pos.x; i++)
    {
        for(int j = -RENDER_DISTANCE + last_player_chunk_pos.y; j < RENDER_DISTANCE+1+last_player_chunk_pos.y; j++)
        {
            for(int k = -RENDER_DISTANCE+last_player_chunk_pos.z; k < RENDER_DISTANCE+1+last_player_chunk_pos.z; k++)
            {
                if(std::find_if(m_loaded_chunks.begin(), m_loaded_chunks.end(), [&i, &j, &k](const Chunk& c){ return c.pos == glm::vec3{i, j, k};})
                    == m_loaded_chunks.end())
                {
                    //make new chunk where old needs to be deeleted
                    reassignable_chunks[current_index]->~Chunk();
                    new (reassignable_chunks[current_index]) Chunk(i, j, k, *this);
                    current_index++;
                }
            }
        }
    }

    //this is the slowest part
    for(int i = 0; i < reassignable_chunks.size(); i++)
    {
        Chunk& c = *reassignable_chunks[i];
        c.generate_all_faces();
    }

    //in this function so it can run on a separate thread
    reload_geometry();

    m_to_reload = true;
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
