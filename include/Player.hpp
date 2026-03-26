#pragma once
#include "Camera.hpp"


class Player
{
public:
    Player();
    void handle_keyboard(GLFWwindow* window, float delta_time);
    void set_position(const glm::vec3& new_pos);
    const glm::vec3 get_position() { return m_position; }

    glm::mat4 get_transform() { return m_camera.get_transform(); }
private:
    Camera m_camera;
    float m_speed;
    glm::vec3 m_position;
};