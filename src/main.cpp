#include "World.hpp"
#include "Camera.hpp"
#include "OpenGL_support/ShaderProgram.hpp"
#include "OpenGL_support/VAO.hpp"
#include "OpenGL_support/VBO.hpp"
#include "OpenGL_support/VertexBufferLayout.hpp"
#include "Renderer.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

void process_input(GLFWwindow* window, Camera& cam)
 {

    glm::vec3 move_vec{0, 0, 0};

    if(glfwGetKey(window, GLFW_KEY_W))
    {
        move_vec.x = 1;
    }else if(glfwGetKey(window, GLFW_KEY_S))
    {
        move_vec.x = -1;
    }
    if(glfwGetKey(window, GLFW_KEY_A))
    {
        move_vec.z = -1;
    }else if(glfwGetKey(window, GLFW_KEY_D))
    {
        move_vec.z = 1;
    }

    cam.move_relative(glm::normalize(move_vec));
 }

int main(){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Minecraft", NULL, NULL);

    if(window == NULL) return 0;

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "glad failed\n";
        return 0;
    }

    float vertices[] = {
        0, 0.5,
        -0.5, -0.5,
        0.5, -0.5
    };

    {
    ShaderProgram program("shaders/basic_triangle.vert", "shaders/basic_triangle.frag");

    //std::cout << glGetError() << "\n";
    glfwSwapInterval(0);

    Camera cam;
    cam.position = {0, 0, 0};
    cam.speed = 8;
    Renderer r;
    World world(r);

    const int R = 64;

    /*glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);*/  

    double delta_time = 0;

    while(!glfwWindowShouldClose(window))
    {

        double start = glfwGetTime();

        glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //UPDATE LOOP
        cam.handle_keyboard(window, delta_time);


        //RENDER CODE
        program.bind();
        program.set_uniform("transform", cam.get_transform());
        world.render(r);

        glfwSwapBuffers(window);
        glfwPollEvents();

        double end = glfwGetTime();

        delta_time = end - start;
        //std::cout << "Frametime: " << deltaTime << "\tFPS: " << 1/deltaTime << "\n";
    }

    }
    glfwTerminate();

    return 0;
}