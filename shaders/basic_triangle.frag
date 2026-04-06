#version 450 core

out vec4 FragColor;

in vec3 outColor;
in vec2 textureCoords;
in float texID;

uniform sampler2D u_texture;

//atlas data
uniform int u_textures_per_side;
uniform int u_atlas_width;
uniform int u_atlas_height;


//converst coords (0, 1) to proper atlas coords
vec2 coords_to_atlas(vec2 coords)
{

    int texture_id = int(texID);

    int texture_x = (texture_id % u_textures_per_side) / u_textures_per_side;
    int texture_y = (texture_id / u_textures_per_side) / u_textures_per_side;

    vec2 atlas_coords = coords / u_textures_per_side; 
    atlas_coords += vec2(texture_x, texture_y);

    float pixel_w = 1 / u_atlas_width;
    float pixel_h = 1 / u_atlas_height;

    //half pixel correction    program.set_uniform("u_atlas_width", (int)block_atlas.get_atlas_w());

    atlas_coords -= 0.5*vec2(pixel_w, pixel_h);

    return atlas_coords;
}

void main()
{

    //textures start from bottom left to top right

    FragColor = texture(u_texture, coords_to_atlas(textureCoords));
}