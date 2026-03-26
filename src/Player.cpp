#include "Player.hpp"
#include <GLFW/glfw3.h>

Player::Player()
:   m_camera(),
    m_speed(8.f)
{}

void Player::handle_keyboard(GLFWwindow* window, float delta_time)
{
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
}

void Player::set_position(const glm::vec3 &new_pos)
{
    m_position = new_pos;
    m_camera.position = m_position + m_camera.up*1.f;
}
