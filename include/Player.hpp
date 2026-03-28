#pragma once
#include "Camera.hpp"

constexpr int REACH_LENGTH = 5;

class GLFWwindow;
class World;

class Player
{
public:
    Player(World& w);
    void handle_keyboard(GLFWwindow* window, float delta_time);
    void set_position(const glm::vec3& new_pos);
    const glm::vec3 get_position() { return m_position; }

    glm::mat4 get_transform() { return m_camera.get_transform(); }

    //test of look at raytrace
    void raytrace_block();
    void update(GLFWwindow* window, const float dt);
private:
    Camera m_camera;
    float m_speed;
    glm::vec3 m_position;
    World& m_world;
};