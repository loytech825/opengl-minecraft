#pragma once
#include <glm/glm.hpp>

class GLFWwindow;

class Camera
{
public:

    Camera();

    glm::vec3 position;

    float fov;
    //+x = yaw=0
    float yaw;
    float pitch;

    const glm::mat4 get_transform();

private:
    friend class Player;
    glm::mat4 proj;
    glm::mat4 view;

    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 world_up;
    glm::vec3 up;

    void update_vectors();
    //TODO: pos to index

};