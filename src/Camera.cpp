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