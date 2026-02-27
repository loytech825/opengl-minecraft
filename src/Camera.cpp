#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
:   pos(0, 0, 0),
    look_at(0, 0, 0),
    fov(90)
{}

const glm::mat4 Camera::get_transform()
{
    proj = glm::perspective(glm::radians(fov), (float)800 / (float)600, 0.1f, 100.0f);
    view = glm::lookAt(pos, look_at, glm::vec3(0, 1, 0));

    return proj*view;
}