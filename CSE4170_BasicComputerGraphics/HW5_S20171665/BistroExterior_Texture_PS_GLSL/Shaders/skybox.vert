#version 400

layout (location = 0) in vec3 a_position;
out vec3 v_tex_coord;

uniform mat4 u_ModelViewProjectionMatrix;

void main(void) {	
	v_tex_coord = a_position;

	gl_Position = u_ModelViewProjectionMatrix * vec4(a_position, 1.0f);
}