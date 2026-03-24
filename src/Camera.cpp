#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>



Camera::Camera()
:   position(0, 0, 0),
    fov(90),
    world_up(0, 1, 0),
    yaw(0),
    pitch(0)
{
    update_vectors();
}

const glm::mat4 Camera::get_transform()
{
    proj = glm::perspective(glm::radians(fov), (float)800 / (float)600, 0.1f, 100.0f);
    view = glm::lookAt(position, position+front, glm::vec3(0, 1, 0));

    return proj*view;
}

void Camera::update_vectors()
{
    front.x = cos(yaw) * cos(pitch);
    front.z = sin(yaw) * cos(pitch);
    front.y = sin(pitch);

    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::handle_keyboard(GLFWwindow *window, float deltaTime)
{
    float velocity = speed * deltaTime;
    glm::vec3 direction = glm::normalize(glm::vec3(front.x, 0, front.z));

    //movement
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        position += velocity*direction;
    }else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        position -= velocity*direction;
    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        position += velocity*right;
    }else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        position -= velocity*right;
    }

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        position += velocity*world_up;
    }else if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        position -= velocity*world_up;
    }

    //yaw
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        yaw += 1.6f * deltaTime;
    }else if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        yaw -= 1.6f * deltaTime;
    }

    //pitch
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        pitch += 1.6f * deltaTime;
    }else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        pitch -= 1.6f * deltaTime;
    }

    if(pitch > glm::radians(89.0f)) pitch = glm::radians(89.0f);
    else if (pitch < glm::radians(-89.0f)) pitch = glm::radians(-89.0f);

    update_vectors();
}