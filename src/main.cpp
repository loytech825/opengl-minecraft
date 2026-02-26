#include "World.hpp"
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
    VBO buffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, 6*sizeof(float));

    VertexBufferLayout layout;
    layout.push<float>(2, false);

    VAO vertex_array;
    vertex_array.add_buffer(buffer, layout);

    ShaderProgram program("shaders/basic_triangle.vert", "shaders/basic_triangle.frag");

    //std::cout << glGetError() << "\n";
    glfwSwapInterval(0);

    while(!glfwWindowShouldClose(window))
    {

        glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //RENDER CODE
        program.bind();
        vertex_array.bind();

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
        //std::cout << "Frametime: " << deltaTime << "\tFPS: " << 1/deltaTime << "\n";
    }

    }
    glfwTerminate();

    return 0;
}