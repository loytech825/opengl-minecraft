#include "World.hpp"
#include "Camera.hpp"
#include "OpenGL_support/ShaderProgram.hpp"
#include "OpenGL_support/VAO.hpp"
#include "OpenGL_support/VBO.hpp"
#include "OpenGL_support/VertexBufferLayout.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

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

    //World world;

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
    World world;

    cam.look_at.y = 8;

    const int R = 5;

    while(!glfwWindowShouldClose(window))
    {

        glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //UPDATE LOOP
        cam.pos.x = glm::cos(glfwGetTime()*0.5)*R;
        cam.pos.z = glm::sin(glfwGetTime()*0.5)*R;
        cam.pos.y = 8;


        //RENDER CODE
        program.bind();
        program.set_uniform("transform", cam.get_transform());
        world.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
        //std::cout << "Frametime: " << deltaTime << "\tFPS: " << 1/deltaTime << "\n";
    }

    }
    glfwTerminate();

    return 0;
}