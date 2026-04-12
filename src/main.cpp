#include "World.hpp"
#include "Camera.hpp"
#include "OpenGL_support/ShaderProgram.hpp"
#include "OpenGL_support/VAO.hpp"
#include "OpenGL_support/VBO.hpp"
#include "OpenGL_support/VertexBufferLayout.hpp"
#include "Renderer.hpp"
#include "Player.hpp"
#include "TextureAtlas.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"

#include <iostream>
#include <filesystem>

//temp3 texture loading
unsigned int load_texture(const std::string& path)
{
    /*TEMP texture loading*/
    unsigned int texture1;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(std::filesystem::path(path).c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    std::cout << path << ": " << width << "x" << height << ", " << nrChannels << "\n";

    return texture1;
}

int main(){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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

    Renderer r;
    World world(r);
    Player player(world);
    player.set_position({0, 0, 0});

    double delta_time = 0;

    //textures
    TextureAtlas block_atlas({"textures/test.png", "textures/stone.png"}, 16, 16);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, block_atlas.get_handle());

    glEnable(GL_DEPTH_TEST);

    program.bind();
    program.set_uniform("u_texture", 0);

    while(!glfwWindowShouldClose(window))
    {

        double start = glfwGetTime();

        glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //UPDATE LOOP
        player.update(window, delta_time);
        world.update(delta_time, player);


        //RENDER CODE

        program.bind();
        program.set_uniform("transform", player.get_transform());
        world.render();

        r.init_batch();
        player.draw(&r);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        r.flush();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glfwSwapBuffers(window);
        glfwPollEvents();

        double end = glfwGetTime();

        delta_time = end - start;
    }

    }
    glfwTerminate();

    return 0;
}