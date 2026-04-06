#include "TextureAtlas.hpp"

#include <cmath>
#include <filesystem>
#include <stb_image.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

TextureAtlas::TextureAtlas(const std::vector<std::string>& textures, int img_w, int img_h)
{

    //total number of pixels
    unsigned int px_count = img_w*img_h*textures.size();

    float atlas_size = sqrt(textures.size());

    //get int part of atlas size
    //we make a sqare large enough to hold all the textures
    m_side_textures = ceil(atlas_size);

    unsigned int atlas_width = img_w*m_side_textures;
    unsigned int atlas_height = img_h*m_side_textures;

    m_atlas_w = atlas_width;
    m_atlas_h = atlas_height;

    std::vector<unsigned char> atlas_data;
    //we will be storing in RGBA format
    atlas_data.reserve(atlas_width*atlas_height*4);

    //load each texture and store its pixels
    for(int i = 0; i < textures.size(); i++)
    {
        int width, height, nr_channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(std::filesystem::path(textures[i]).c_str(), &width, &height, &nr_channels, 0);

        //here width and height should match img_w and img_h respectively

        int start_x = (i%atlas_width) * img_w;
        int start_y = (i/atlas_width) * img_h;

        for(int px_x = 0; px_x < img_w; px_x++){
            for(int px_y = 0; px_y < img_h; px_y++){

                int index = (px_y*img_w + px_x) * nr_channels;

                int atlas_index_R = ((start_y+ px_y) *atlas_width + start_x+px_x)*4;

                //need to do r, g, b, and a
                atlas_data[atlas_index_R] = data[index];
                atlas_data[atlas_index_R + 1] = data[index + 1];
                atlas_data[atlas_index_R + 2] = data[index + 2];
                //set alpha to img alpha if exists otherwise to full 255
                atlas_data[atlas_index_R + 3] = (nr_channels == 4) ? data[index + 3] : 255;
            }
        }
        stbi_image_free(data);
    }

    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_2D, m_handle); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas_width, atlas_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas_data.data());
    //glGenerateMipmap(GL_TEXTURE_2D);
}