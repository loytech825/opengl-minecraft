#include <glm/glm.hpp>

class GLFWwindow;

class Camera
{
public:

    Camera();

    void handle_keyboard(GLFWwindow* window, float delta_time);

    glm::vec3 position;

    float fov;
    float yaw;
    float pitch;
    
    float speed;

    const glm::mat4 get_transform();
    /*
    @brief Moves the camera relative to its looking direction
    @param direction - unit vector
    */
    void move_relative(const glm::vec3& direction);

private:
    glm::mat4 proj;
    glm::mat4 view;

    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 world_up;
    glm::vec3 up;

    void update_vectors();

};