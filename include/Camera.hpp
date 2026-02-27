#include <glm/glm.hpp>

class Camera
{
public:

    Camera();

    glm::vec3 pos;
    glm::vec3 look_at;

    float fov;

    const glm::mat4 get_transform();

private:
    glm::mat4 proj;
    glm::mat4 view;

};