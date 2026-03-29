#include "Player.hpp"
#include "World.hpp"
#include "Chunk.hpp"
#include <GLFW/glfw3.h>

#include <iostream>

Player::Player(World& w)
:   m_camera(),
    m_speed(8.f),
    m_world(w),
    m_targeted_block(nullptr)
{}

void Player::handle_keyboard(GLFWwindow* window, float delta_time)
{
    /****************CAMERA*UPDATE*****************/
    float velocity = m_speed * delta_time;
    glm::vec3 direction = glm::normalize(glm::vec3(m_camera.front.x, 0, m_camera.front.z));

    //movement
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        m_position += velocity*direction;
    }else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        m_position -= velocity*direction;
    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        m_position += velocity*m_camera.right;
    }else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        m_position -= velocity*m_camera.right;
    }

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        m_position += velocity*m_camera.world_up;
    }else if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        m_position -= velocity*m_camera.world_up;
    }

    //yaw
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        m_camera.yaw += 1.6f * delta_time;
    }else if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        m_camera.yaw -= 1.6f * delta_time;
    }

    //pitch
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        m_camera.pitch += 1.6f * delta_time;
    }else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        m_camera.pitch -= 1.6f * delta_time;
    }

    if(m_camera.pitch > glm::radians(89.0f)) m_camera.pitch = glm::radians(89.0f);
    else if (m_camera.pitch < glm::radians(-89.0f)) m_camera.pitch = glm::radians(-89.0f);

    m_camera.position = m_position + m_camera.up*1.f;
    m_camera.update_vectors();

    /***************BLOCK*INTERACTION******************/
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if(m_targeted_block)
        {
            m_world.set_block(m_targeted_block_pos, Block(AIR));
        }
    }
}

void Player::set_position(const glm::vec3 &new_pos)
{
    m_position = new_pos;
    m_camera.position = m_position + m_camera.up*1.f;
}

void Player::update(GLFWwindow* window, const float dt)
{
    handle_keyboard(window, dt);
    raytrace_block();
}

// made with referencing 
// lodev.org/cgtutor/raycasting.html

std::string vec_to_str(const glm::vec3& vector)
{
    return "" + std::to_string(vector.x) + ", " + std::to_string(vector.y) + ", " + std::to_string(vector.z);
}

void Player::raytrace_block()
{
    //first we need the looking direction vector
    const auto ray = m_camera.front;
    const auto max_distance = ray*(float)REACH_LENGTH;

    glm::vec3 int_pos = glm::floor(m_camera.position);

    //std::cout << "Looking vector: " << ray.x << ", " << ray.y << ", " << ray.z << "\n";
    //std::cout << "Max distance vector: " << max_distance.x << ", " << max_distance.y << ", " << max_distance.z << "\n";

    //calculate how long the ray is between each block in x, y or z directions
    float delta_dist_X = (ray.x == 0) ? 1e30 : std::abs(1/ray.x);
    float delta_dist_Y = (ray.y == 0) ? 1e30 : std::abs(1/ray.y);
    float delta_dist_Z = (ray.z == 0) ? 1e30 : std::abs(1/ray.z);

    //distances from cuurent location to nearest sides
    float side_dist_X, side_dist_Y, side_dist_Z;
    //either +1 or -1
    signed char step_X, step_Y, step_Z;

    //gets the distance to nearest face in each direction and wether we are going + or - for each direction 
    if(ray.x < 0)   {side_dist_X = (m_position.x - int_pos.x)*delta_dist_X;      step_X = -1;}
    else            {side_dist_X = (int_pos.x + 1 - m_position.x)*delta_dist_X;  step_X = 1;}

    //if this happens we are right on the edge, so we move 1 block over
    if(side_dist_X == 0)
    { int_pos.x += step_X; side_dist_X += delta_dist_X; }

    if(ray.y < 0)   {side_dist_Y = (m_position.y - int_pos.y)*delta_dist_Y;      step_Y = -1;}
    else            {side_dist_Y = (int_pos.y + 1 - m_position.y)*delta_dist_Y;  step_Y = 1;}

    if(side_dist_Y == 0)
    { int_pos.y += step_Y; side_dist_Y += delta_dist_Y; }

    if(ray.z < 0)   {side_dist_Z = (m_position.z - int_pos.z)*delta_dist_Z;      step_Z = -1;}
    else            {side_dist_Z = (int_pos.z + 1 - m_position.z)*delta_dist_Z;  step_Z = 1;}

    if(side_dist_Y == 0)
    { int_pos.y += step_Y; side_dist_Y += delta_dist_Y; }

    glm::vec3 side_dist{side_dist_X, side_dist_Y, side_dist_Z};

    float amount_traveled = 0;

    //TODO: add check for block we're in

    //side dist keeps track of distance from player pos to next side to be checked
    //std::cout << "Pos: " << vec_to_str(m_position) << "\n";
    //std::cout << "IntPos: " << vec_to_str(int_pos) << "\n";
    //std::cout << "Direction: " << vec_to_str(ray) << "\n";
    //std::cout << "Side_dist: " << vec_to_str(side_dist) << "\n";

    //when we are exactly at the edge of a block, the side we are on has a distance of 0, which can be a problem
    const Block* looking_at = nullptr;
    looking_at = m_world.get_block(int_pos);

    while(amount_traveled < REACH_LENGTH)
    {
        if(looking_at && looking_at->type != 0) break;

        //x side is closest so we move there
        if(side_dist.x < side_dist.y && side_dist.x < side_dist.z)
        {
            //we move for one block in X direction so the ray gets delta_dist_X longer
            side_dist.x += delta_dist_X;
            int_pos.x += step_X;  
            amount_traveled += delta_dist_X;
            //std::cout << "X\n";
        }
        //y closest
        else if (side_dist.y < side_dist.z)
        {
            side_dist.y += delta_dist_Y;
            int_pos.y += step_Y;
            amount_traveled += delta_dist_Y;
            //std::cout << "Y\n";
        }
        //z closest
        else
        {
            side_dist.z += delta_dist_Z;
            int_pos.z += step_Z;
            amount_traveled += delta_dist_Z;
            //std::cout << "Z\n";
        }
        looking_at = m_world.get_block(int_pos);
        //std::cout << "Amount travelled: " << amount_traveled << "\n";
        //std::cout << "Block found: " << int_pos.x << ", " << int_pos.y << ", " << int_pos.z << "\n"; 
    }

    m_targeted_block = looking_at;
    m_targeted_block_pos = int_pos;

    /*std::cout << "Looking at: ";
    if(looking_at) std::cout << (unsigned int)looking_at->type << "\n";
    else std::cout << "nothing.\n";*/
}
