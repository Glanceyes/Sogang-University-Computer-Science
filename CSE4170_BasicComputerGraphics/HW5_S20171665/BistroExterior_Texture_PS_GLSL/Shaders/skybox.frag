#version 400

in vec3 v_tex_coord;
layout (location = 0) out vec4 final_color;
 
uniform samplerCube u_skymap;

void main(void) {
    final_color = texture(u_skymap,  v_tex_coord); 
}
