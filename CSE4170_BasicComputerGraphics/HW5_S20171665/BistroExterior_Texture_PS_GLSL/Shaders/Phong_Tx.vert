#version 400

uniform mat4 u_ModelViewProjectionMatrix;
uniform mat4 u_ModelViewMatrix;
uniform mat3 u_ModelViewMatrixInvTrans;  

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in vec2 a_temp_coord;

out vec3 v_position_EC;
out vec3 v_normal_EC;
out vec2 v_tex_coord;
out vec2 v_temp_coord;
out vec3 v_position_MC;
out vec3 v_normal_MC;

void main(void) {	
	v_position_EC = vec3(u_ModelViewMatrix*vec4(a_position, 1.0f));
	v_normal_EC = normalize(u_ModelViewMatrixInvTrans*a_normal);  
	v_tex_coord = a_tex_coord;
	v_temp_coord = a_temp_coord;
	
	v_position_MC = a_position;
	v_normal_MC = a_normal;

	gl_Position = u_ModelViewProjectionMatrix*vec4(a_position, 1.0f);
}