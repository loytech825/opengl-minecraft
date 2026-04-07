#pragma once

#include <vector>
#include <string>

class TextureAtlas
{
public:
    //constructs an empty atlas with the size (x, y)
    //TextureAtlas(int x, int y);

    //constructs a texture atlas with images
    //they need to be the same size, namely img_w x img_h
    TextureAtlas(const std::vector<std::string>& textures, int img_w, int img_h);

    unsigned int get_handle() {return m_handle;}

    //atlas is x by x textures, this function returns x
    //unsigned int get_size_textures() {return m_side_textures;}
    //unsigned int get_atlas_w() {return m_atlas_w;};
    //unsigned int get_atlas_h() {return m_atlas_h;};

private:
    //corresponds to the opengl texture id
    unsigned int m_handle;
    //unsigned int m_side_textures;
    //unsigned int m_atlas_w;
    //unsigned int m_atlas_h;
};