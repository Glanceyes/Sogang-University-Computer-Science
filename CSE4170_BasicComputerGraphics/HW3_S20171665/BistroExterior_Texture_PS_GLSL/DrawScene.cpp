//
//  DrawScene.cpp
//
//  Written for CSE4170
//  Department of Computer Science and Engineering
//  Copyright © 2023 Sogang University. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "LoadScene.h"

// Begin of shader setup
#include "Shaders/LoadShaders.h"
#include "ShadingInfo.h"

extern SCENE scene;

// for simple shaders
GLuint h_ShaderProgram_simple; // handle to shader program
GLuint h_ShaderProgram_background, h_ShaderProgram_equiToCube;
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// for PBR
GLuint h_ShaderProgram_TXPBR;
#define NUMBER_OF_LIGHT_SUPPORTED 1
GLint loc_global_ambient_color;
GLint loc_lightCount;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
GLint loc_ModelViewProjectionMatrix_TXPBR, loc_ModelViewMatrix_TXPBR, loc_ModelViewMatrixInvTrans_TXPBR;
GLint loc_cameraPos;

#define TEXTURE_INDEX_DIFFUSE	(0)
#define TEXTURE_INDEX_NORMAL	(1)
#define TEXTURE_INDEX_SPECULAR	(2)
#define TEXTURE_INDEX_EMISSIVE	(3)
#define TEXTURE_INDEX_SKYMAP	(4)

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2

#define GLUT_SCROLL_UP 3
#define GLUT_SCROLL_DOWN 4

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

// for tiger animation
unsigned int timestamp_scene = 0; // the global clock in the scene
unsigned int timestamp_for_tiger = 0; // the clock for the tiger animation
int flag_animation, flag_polygon_fill;
int cur_frame_tiger = 0, cur_frame_ben = 0, cur_frame_wolf, cur_frame_spider = 0;
float rotation_angle_tiger = 0.0f;

// for interactive viewing
int window_width, window_height;
int prev_x, prev_y;
int space_pressed, z_pressed, tiger_view_active, tiger_rear_view_active; 
int left_mouse_pressed, right_mouse_pressed, middle_mouse_pressed;
float z_distance = 1000.0f, z_x, z_y;


#define MAX_Z_DISTANCE 10000.0f
#define MIN_Z_DISTANCE 300.0f

// texture stuffs
#define N_TEXTURES_USED 2
#define TEXTURE_ID_FLOOR 0
#define TEXTURE_ID_TIGER 1
GLuint texture_names[N_TEXTURES_USED];

// for skybox shaders
GLuint h_ShaderProgram_skybox;
GLint loc_cubemap_skybox;
GLint loc_ModelViewProjectionMatrix_SKY;

void update_camera_orientation_20171665(int axis, float rotation_angle);

int read_geometry(GLfloat** object, int bytes_per_primitive, char* filename) {
	int n_triangles;
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}
	fread(&n_triangles, sizeof(int), 1, fp);

	*object = (float*)malloc(n_triangles * bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	fread(*object, bytes_per_primitive, n_triangles, fp);
	//fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}


// include glm/*.hpp only if necessary
// #include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
// ViewProjectionMatrix = ProjectionMatrix * ViewMatrix
glm::mat4 ViewProjectionMatrix, ViewMatrix, ProjectionMatrix;
// ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix
glm::mat4 ModelViewProjectionMatrix; // This one is sent to vertex shader when ready.
glm::mat4 ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;

glm::mat4 TigerModelViewMatrix;
glm::vec3 sphere_position;

#define TO_RADIAN 0.01745329252f
#define PI 3.14159265358979323846f
#define TO_DEGREE 57.295779513f

/*********************************  START: camera *********************************/

#define CAM_TRANSLATION_SPEED 20.0f
#define CAM_ROTATION_SPEED 0.1f
typedef enum {
	X_AXIS, Y_AXIS, Z_AXIS
} CAMERA_AXIS;

typedef enum {
	CAMERA_1,
	CAMERA_2,
	CAMERA_3,
	CAMERA_4,
	CAMERA_5,
	CAMERA_6,
	CAMERA_TIGER,
	CAMERA_TIGER_REAR,
	NUM_CAMERAS
} CAMERA_INDEX;

int current_camera_num;

typedef struct _Camera {
	float pos[3];
	float uaxis[3], vaxis[3], naxis[3];
	float fovy, aspect_ratio, near_c, far_c;
	int move, rotation_axis;
} Camera;

Camera camera_info[NUM_CAMERAS];
Camera current_camera;

using glm::mat4;
void set_view_matrix_from_camera_frame(void) {
	ViewMatrix = glm::mat4(current_camera.uaxis[0], current_camera.vaxis[0], current_camera.naxis[0], 0.0f,
		current_camera.uaxis[1], current_camera.vaxis[1], current_camera.naxis[1], 0.0f,
		current_camera.uaxis[2], current_camera.vaxis[2], current_camera.naxis[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-current_camera.pos[0], -current_camera.pos[1], -current_camera.pos[2]));
}

void rotate_camera_to_point(int x, int y) {
	glm::mat3 ViewMatrixInverse = glm::mat3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2],
		current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2],
		current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]
	);

	glm::vec3 viewDirection = ViewMatrixInverse * glm::vec3(0.0f, 0.0f, -1.0f);
	float near_clip_height = 2.0f * current_camera.near_c * glm::tan(current_camera.fovy /2.0f);
	float near_clip_width = near_clip_height * current_camera.aspect_ratio;
	float relative_x = (x - window_width / 2.0f) / window_width * near_clip_width;
	float relative_y = -(y - window_height / 2.0f) / window_height * near_clip_height;
	glm::vec3 clickDirection = ViewMatrixInverse * glm::vec3(relative_x, relative_y, -current_camera.near_c);

	//printf("click position: (%lf, %lf, %lf)\n", relative_x, relative_y, -current_camera.near_c);
	//printf("clickDirection: (%lf, %lf, %lf)\n", clickDirection.x, clickDirection.y, clickDirection.z);
	clickDirection = glm::normalize(clickDirection);
	
	//printf("normalized clickDirection: (%lf, %lf, %lf)\n", clickDirection.x, clickDirection.y, clickDirection.z);

	glm::vec3 axisDirection = glm::normalize(glm::cross(viewDirection, clickDirection));

	float degree = glm::acos(glm::dot(viewDirection, clickDirection));

	//printf("degree: %lf\n", degree / TO_RADIAN);

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), degree, axisDirection);

	glm::vec3 uaxis = glm::mat3(rotationMatrix) * glm::vec3(
		current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);

	glm::vec3 vaxis = glm::mat3(rotationMatrix) * glm::vec3(
		current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]
	);

	glm::vec3 naxis = glm::mat3(rotationMatrix) * glm::vec3(
		current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]
	);

	current_camera.uaxis[0] = uaxis.x;
	current_camera.uaxis[1] = uaxis.y;
	current_camera.uaxis[2] = uaxis.z;

	current_camera.vaxis[0] = vaxis.x;
	current_camera.vaxis[1] = vaxis.y;
	current_camera.vaxis[2] = vaxis.z;

	current_camera.naxis[0] = naxis.x;
	current_camera.naxis[1] = naxis.y;
	current_camera.naxis[2] = naxis.z;

	set_view_matrix_from_camera_frame();
}

void set_current_camera(int camera_num) {
	Camera* pCamera = &camera_info[camera_num];

	memcpy(&current_camera, pCamera, sizeof(Camera));
	current_camera.aspect_ratio = (float)window_width / (float)window_height;
	set_view_matrix_from_camera_frame();
	if (camera_num == CAMERA_TIGER || camera_num == CAMERA_TIGER_REAR) {
		if (flag_animation)  {
			if (camera_num == CAMERA_TIGER) {
				if (timestamp_scene % 60 < 15) {
					update_camera_orientation_20171665(X_AXIS, timestamp_scene % 30);
				}
				else if (timestamp_scene % 60 < 30) {
					update_camera_orientation_20171665(X_AXIS, 30 - timestamp_scene % 30);
				}
				else if (timestamp_scene % 60 < 45) {
					update_camera_orientation_20171665(X_AXIS, -(int)(timestamp_scene % 30));
				}
				else {
					update_camera_orientation_20171665(X_AXIS, -(int)(30 - timestamp_scene % 30));
				}
			}
		}
		set_view_matrix_from_camera_frame();
		ViewMatrix = ViewMatrix * glm::inverse(TigerModelViewMatrix);
	}
	else {
		set_view_matrix_from_camera_frame();
	}
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void initialize_camera(void) {
	//CAMERA_1 : original view
	Camera* pCamera = &camera_info[CAMERA_1];
	for (int k = 0; k < 3; k++)
	{
		pCamera->pos[k] = scene.camera.e[k];
		pCamera->uaxis[k] = scene.camera.u[k];
		pCamera->vaxis[k] = scene.camera.v[k];
		pCamera->naxis[k] = scene.camera.n[k];
	}

	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_2 : arch view
	pCamera = &camera_info[CAMERA_2];
	pCamera->pos[0] = 3686.847412f; pCamera->pos[1] = -2254.680420f; pCamera->pos[2] = 489.447662f;
	pCamera->uaxis[0] = 0.369476f; pCamera->uaxis[1] = 0.929234f; pCamera->uaxis[2] = 0.000406f;
	pCamera->vaxis[0] = -0.173816f; pCamera->vaxis[1] = 0.068682f; pCamera->vaxis[2] = 0.982376f;
	pCamera->naxis[0] = 0.912833f; pCamera->naxis[1] = -0.363037f; pCamera->naxis[2] = 0.186891f;
	pCamera->move = 0;
	pCamera->fovy = 0.837758, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_3 : top view
	pCamera = &camera_info[CAMERA_3];
	pCamera->pos[0] = -2214.219727f; pCamera->pos[1] = 1914.504272f; pCamera->pos[2] = 6471.403320f;
	pCamera->uaxis[0] = -0.484424f; pCamera->uaxis[1] = -0.874237f; pCamera->uaxis[2] = -0.032219f;
	pCamera->vaxis[0] = 0.690941f; pCamera->vaxis[1] = -0.404927f; pCamera->vaxis[2] = 0.598860f;
	pCamera->naxis[0] = -0.536594f; pCamera->naxis[1] = 0.267840f; pCamera->naxis[2] = 0.800201f;
	pCamera->move = 0;
	pCamera->fovy = 0.837758, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_4 : tree view
	pCamera = &camera_info[CAMERA_4];
	pCamera->pos[0] = 2543.753906f; pCamera->pos[1] = -1820.367065f; pCamera->pos[2] = 1076.705688f;
	pCamera->uaxis[0] = -0.329894f; pCamera->uaxis[1] = -0.944011f; pCamera->uaxis[2] = -0.000716f;
	pCamera->vaxis[0] = 0.279950f; pCamera->vaxis[1] = -0.098555f; pCamera->vaxis[2] = 0.954935f;
	pCamera->naxis[0] = -0.901544f; pCamera->naxis[1] = 0.314829f; pCamera->naxis[2] = 0.296790f;
	pCamera->move = 0;
	pCamera->fovy = 1.029745, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_5 : cross view
	pCamera = &camera_info[CAMERA_5];
	pCamera->pos[0] = -1340.848022f; pCamera->pos[1] = -461.747070f; pCamera->pos[2] = 2181.886719f;
	pCamera->uaxis[0] = 0.471348f; pCamera->uaxis[1] = -0.881649f; pCamera->uaxis[2] = -0.022423f;
	pCamera->vaxis[0] = 0.546717f; pCamera->vaxis[1] = 0.272148f; pCamera->vaxis[2] = 0.791841f;
	pCamera->naxis[0] = -0.692024f; pCamera->naxis[1] = -0.385493f; pCamera->naxis[2] = 0.610294f;
	pCamera->move = 0;
	pCamera->fovy = 0.959931, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_6 : animation mode view
	pCamera = &camera_info[CAMERA_6];
	pCamera->pos[0] = 139.323364f; pCamera->pos[1] = 1070.164734f; pCamera->pos[2] = 473.017883f;
	pCamera->uaxis[0] = 1.0f; pCamera->uaxis[1] = 0.0f; pCamera->uaxis[2] = 0.0f;
	pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 0.0f; pCamera->vaxis[2] = 1.0f;
	pCamera->naxis[0] = 0.0f; pCamera->naxis[1] = -1.0f; pCamera->naxis[2] = 0.0f;
	pCamera->move = 0;
	pCamera->fovy = 1.099558, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;


	//CAMERA_TIGER: tiger view 
	pCamera = &camera_info[CAMERA_TIGER];
	pCamera->pos[0] = 0.0f; pCamera->pos[1] = -88.0f; pCamera->pos[2] = 62.0f;
	pCamera->uaxis[0] = -1.0f; pCamera->uaxis[1] = 0.0f; pCamera->uaxis[2] = 0.0f;
	pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 0.0f; pCamera->vaxis[2] = 1.0f;
	pCamera->naxis[0] = 0.0f; pCamera->naxis[1] = 1.0f; pCamera->naxis[2] = 0.0f;
	pCamera->move = 0;
	pCamera->fovy = 1.0, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;


	//CAMERA_TIGER_REAR: tiger rear view
	pCamera = &camera_info[CAMERA_TIGER_REAR];
	pCamera->pos[0] = 0.0f; pCamera->pos[1] = 120.0f; pCamera->pos[2] = 80.0f;
	pCamera->uaxis[0] = -1.0f; pCamera->uaxis[1] = 0.0f; pCamera->uaxis[2] = 0.0f;
	pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 0.0f; pCamera->vaxis[2] = 1.0f;
	pCamera->naxis[0] = 0.0f; pCamera->naxis[1] = 1.0f; pCamera->naxis[2] = 0.0f;

	pCamera->move = 0;
	pCamera->fovy = 1.0, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;


	current_camera_num = CAMERA_1;
	set_current_camera(current_camera_num);
}

void update_view_projection_matrix_20171665() {
	ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void update_camera_position_20171665(int translation_axis, float translation_factor) {
	switch (translation_axis) {
	case X_AXIS:
		current_camera.pos[0] += CAM_TRANSLATION_SPEED * translation_factor * current_camera.uaxis[0];
		current_camera.pos[1] += CAM_TRANSLATION_SPEED * translation_factor * current_camera.uaxis[1];
		current_camera.pos[2] += CAM_TRANSLATION_SPEED * translation_factor * current_camera.uaxis[2];
		break;
	case Y_AXIS:
		current_camera.pos[0] += CAM_TRANSLATION_SPEED * translation_factor * current_camera.vaxis[0];
		current_camera.pos[1] += CAM_TRANSLATION_SPEED * translation_factor * current_camera.vaxis[1];
		current_camera.pos[2] += CAM_TRANSLATION_SPEED * translation_factor * current_camera.vaxis[2];
	case Z_AXIS:
		current_camera.pos[0] += CAM_TRANSLATION_SPEED * translation_factor * current_camera.naxis[0];
        current_camera.pos[1] += CAM_TRANSLATION_SPEED * translation_factor * current_camera.naxis[1];
        current_camera.pos[2] += CAM_TRANSLATION_SPEED * translation_factor * current_camera.naxis[2];
        break;
	}
}

void update_camera_orientation_around_u_axis_20171655(float rotation_angle) {
	   glm::mat3 RotationMatrix;
    glm::vec3 direction;
	RotationMatrix = glm::mat3(
        glm::rotate(glm::mat4(1.0f), CAM_ROTATION_SPEED * rotation_angle * TO_RADIAN,
			glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2])
		)
    );

	direction = RotationMatrix * glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]);
	current_camera.vaxis[0] = direction.x; current_camera.vaxis[1] = direction.y; current_camera.vaxis[2] = direction.z;

    direction = RotationMatrix * glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
    current_camera.naxis[0] = direction.x; current_camera.naxis[1] = direction.y; current_camera.naxis[2] = direction.z;
}

void update_camera_orientation_around_v_axis_20171665(float rotation_angle) {
	glm::mat3 RotationMatrix;
	glm::vec3 direction;

	RotationMatrix = glm::mat3(
		glm::rotate(glm::mat4(1.0f), CAM_ROTATION_SPEED * rotation_angle * TO_RADIAN,
			glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2])
		)
	);

	direction = RotationMatrix * glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);
	current_camera.uaxis[0] = direction.x; current_camera.uaxis[1] = direction.y; current_camera.uaxis[2] = direction.z;

	direction = RotationMatrix * glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
	current_camera.naxis[0] = direction.x; current_camera.naxis[1] = direction.y; current_camera.naxis[2] = direction.z;
}

void update_camera_orientation_around_n_axis_20171665(float rotation_angle) {
	glm::mat3 RotationMatrix;
    glm::vec3 direction;
	RotationMatrix = glm::mat3(
        glm::rotate(glm::mat4(1.0f), CAM_ROTATION_SPEED * rotation_angle * TO_RADIAN,
			glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2])
		)
    );

    direction = RotationMatrix * glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);
    current_camera.uaxis[0] = direction.x; current_camera.uaxis[1] = direction.y; current_camera.uaxis[2] = direction.z;
    
	direction = RotationMatrix * glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]);
    current_camera.vaxis[0] = direction.x; current_camera.vaxis[1] = direction.y; current_camera.vaxis[2] = direction.z;
}

void update_camera_orientation_20171665(int axis, float rotation_angle) {
	switch (axis) {
	case X_AXIS:
		update_camera_orientation_around_u_axis_20171655(rotation_angle);
		break;
	case Y_AXIS:
		update_camera_orientation_around_v_axis_20171665(rotation_angle);
		break;
	case Z_AXIS:
		update_camera_orientation_around_n_axis_20171665(rotation_angle);
		break;
	}
}

void rotate_camera_by_sphere(float dx, float dy) {
	float distance_camera_sphere = sqrt(
		pow(current_camera.pos[0] - sphere_position.x, 2) + pow(current_camera.pos[1] - sphere_position.y, 2) + pow(current_camera.pos[2] - sphere_position.z, 2)
	);

	update_camera_orientation_20171665(Y_AXIS, -dx);
	update_camera_orientation_20171665(X_AXIS, -dy);

	glm::vec3 viewing_direction = glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
	viewing_direction = glm::normalize(viewing_direction);

	current_camera.pos[0] = sphere_position.x + viewing_direction.x * distance_camera_sphere;
	current_camera.pos[1] = sphere_position.y + viewing_direction.y * distance_camera_sphere;
	current_camera.pos[2] = sphere_position.z + viewing_direction.z * distance_camera_sphere;
}

void print_camera_information_20171665() {
	printf("current_camera_pos: (%lf %lf %lf)\n", current_camera.pos[0], current_camera.pos[1], current_camera.pos[2]);
	printf("current_camera_uaxis: (%lf %lf %lf)\n", current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);
	printf("current_camera_vaxis: (%lf %lf %lf)\n", current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]);
	printf("current_camera_naxis: (%lf %lf %lf)\n", current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
	printf("current_camera_fovy: %lf\n", current_camera.fovy);
	printf("current_camera_aspect: %lf\n", current_camera.aspect_ratio);
}

/*********************************  END: camera *********************************/

/******************************  START: shader setup ****************************/
// Begin of Callback function definitions
void prepare_shader_program(void) {
	char string[256];

	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram_simple);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");

	ShaderInfo shader_info_TXPBR[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Background/PBR_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Background/PBR_Tx.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_TXPBR = LoadShaders(shader_info_TXPBR);
	glUseProgram(h_ShaderProgram_TXPBR);

	loc_ModelViewProjectionMatrix_TXPBR = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPBR = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPBR = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_ModelViewMatrixInvTrans");

	loc_lightCount = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_light_count");

	for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPBR, string);
		sprintf(string, "u_light[%d].color", i);
		loc_light[i].color = glGetUniformLocation(h_ShaderProgram_TXPBR, string);
	}

	loc_cameraPos = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_camPos");

	//Textures
	loc_material.diffuseTex = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_albedoMap");
	loc_material.normalTex = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_normalMap");
	loc_material.specularTex = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_metallicRoughnessMap");
	loc_material.emissiveTex = glGetUniformLocation(h_ShaderProgram_TXPBR, "u_emissiveMap");

	ShaderInfo shader_info_skybox[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Background/skybox.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Background/skybox.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_skybox = LoadShaders(shader_info_skybox);
	loc_cubemap_skybox = glGetUniformLocation(h_ShaderProgram_skybox, "u_skymap");
	loc_ModelViewProjectionMatrix_SKY = glGetUniformLocation(h_ShaderProgram_skybox, "u_ModelViewProjectionMatrix");
}
/*******************************  END: shder setup ******************************/

/****************************  START: geometry setup ****************************/
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define INDEX_VERTEX_POSITION	0
#define INDEX_NORMAL			1
#define INDEX_TEX_COORD			2



bool b_draw_grid = false;

//axes
GLuint axes_VBO, axes_VAO;
GLfloat axes_vertices[6][3] = {
	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };

void prepare_axes(void) {
	// Initialize vertex buffer object.
	glGenBuffers(1, &axes_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), &axes_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &axes_VAO);
	glBindVertexArray(axes_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	fprintf(stdout, " * Loaded axes into graphics memory.\n");
}

void draw_axes(void) {
	if (!b_draw_grid)
		return;

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(8000.0f, 8000.0f, 8000.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	glBindVertexArray(axes_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glLineWidth(1.0f);
	glUseProgram(0);
}

//grid
#define GRID_LENGTH			(100)
#define NUM_GRID_VETICES	((2 * GRID_LENGTH + 1) * 4)
GLuint grid_VBO, grid_VAO;
GLfloat grid_vertices[NUM_GRID_VETICES][3];
GLfloat grid_color[3] = { 0.5f, 0.5f, 0.5f };

void prepare_grid(void) {

	//set grid vertices
	int vertex_idx = 0;
	for (int x_idx = -GRID_LENGTH; x_idx <= GRID_LENGTH; x_idx++)
	{
		grid_vertices[vertex_idx][0] = x_idx;
		grid_vertices[vertex_idx][1] = -GRID_LENGTH;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;

		grid_vertices[vertex_idx][0] = x_idx;
		grid_vertices[vertex_idx][1] = GRID_LENGTH;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;
	}

	for (int y_idx = -GRID_LENGTH; y_idx <= GRID_LENGTH; y_idx++)
	{
		grid_vertices[vertex_idx][0] = -GRID_LENGTH;
		grid_vertices[vertex_idx][1] = y_idx;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;

		grid_vertices[vertex_idx][0] = GRID_LENGTH;
		grid_vertices[vertex_idx][1] = y_idx;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;
	}

	// Initialize vertex buffer object.
	glGenBuffers(1, &grid_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, grid_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grid_vertices), &grid_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &grid_VAO);
	glBindVertexArray(grid_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, grid_VAO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	fprintf(stdout, " * Loaded grid into graphics memory.\n");
}

void draw_grid(void) {
	if (!b_draw_grid)
		return;

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(1.0f);
	glBindVertexArray(grid_VAO);
	glUniform3fv(loc_primitive_color, 1, grid_color);
	glDrawArrays(GL_LINES, 0, NUM_GRID_VETICES);
	glBindVertexArray(0);
	glLineWidth(1.0f);
	glUseProgram(0);
}

// bistro_exterior
GLuint* bistro_exterior_VBO;
GLuint* bistro_exterior_VAO;
int* bistro_exterior_n_triangles;
int* bistro_exterior_vertex_offset;
GLfloat** bistro_exterior_vertices;
GLuint* bistro_exterior_texture_names;

int flag_fog;
bool* flag_texture_mapping;

void initialize_lights(void) { // follow OpenGL conventions for initialization
	glUseProgram(h_ShaderProgram_TXPBR);

	glUniform1f(loc_lightCount, scene.n_lights);

	for (int i = 0; i < scene.n_lights; i++) {
		glUniform4f(loc_light[i].position,
			scene.light_list[i].pos[0],
			scene.light_list[i].pos[1],
			scene.light_list[i].pos[2],
			0.0f);

		glUniform3f(loc_light[i].color,
			scene.light_list[i].color[0],
			scene.light_list[i].color[1],
			scene.light_list[i].color[2]);
	}

	glUseProgram(0);
}

bool readTexImage2D_from_file(char* filename) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap, * tx_pixmap_32;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	if (tx_pixmap == NULL)
		return false;
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	//fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);
	GLenum format, internalFormat;
	if (tx_bits_per_pixel == 32) {
		format = GL_BGRA;
		internalFormat = GL_RGBA;
	}
	else if (tx_bits_per_pixel == 24) {
		format = GL_BGR;
		internalFormat = GL_RGB;
	}
	else {
		fprintf(stdout, " * Converting texture from %d bits to 32 bits...\n", tx_bits_per_pixel);
		tx_pixmap = FreeImage_ConvertTo32Bits(tx_pixmap);
		format = GL_BGRA;
		internalFormat = GL_RGBA;
	}

	width = FreeImage_GetWidth(tx_pixmap);
	height = FreeImage_GetHeight(tx_pixmap);
	data = FreeImage_GetBits(tx_pixmap);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	//fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap);

	return true;
}

void prepare_bistro_exterior(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	// VBO, VAO malloc
	bistro_exterior_VBO = (GLuint*)malloc(sizeof(GLuint) * scene.n_materials);
	bistro_exterior_VAO = (GLuint*)malloc(sizeof(GLuint) * scene.n_materials);

	bistro_exterior_n_triangles = (int*)malloc(sizeof(int) * scene.n_materials);
	bistro_exterior_vertex_offset = (int*)malloc(sizeof(int) * scene.n_materials);

	flag_texture_mapping = (bool*)malloc(sizeof(bool) * scene.n_textures);

	// vertices
	bistro_exterior_vertices = (GLfloat**)malloc(sizeof(GLfloat*) * scene.n_materials);

	for (int materialIdx = 0; materialIdx < scene.n_materials; materialIdx++) {
		MATERIAL* pMaterial = &(scene.material_list[materialIdx]);
		GEOMETRY_TRIANGULAR_MESH* tm = &(pMaterial->geometry.tm);

		// vertex
		bistro_exterior_vertices[materialIdx] = (GLfloat*)malloc(sizeof(GLfloat) * 8 * tm->n_triangle * 3);

		int vertexIdx = 0;
		for (int triIdx = 0; triIdx < tm->n_triangle; triIdx++) {
			TRIANGLE tri = tm->triangle_list[triIdx];
			for (int triVertex = 0; triVertex < 3; triVertex++) {
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].x;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].y;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].z;

				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].x;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].y;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].z;

				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.texture_list[triVertex][0].u;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.texture_list[triVertex][0].v;
			}
		}

		// # of triangles
		bistro_exterior_n_triangles[materialIdx] = tm->n_triangle;

		if (materialIdx == 0)
			bistro_exterior_vertex_offset[materialIdx] = 0;
		else
			bistro_exterior_vertex_offset[materialIdx] = bistro_exterior_vertex_offset[materialIdx - 1] + 3 * bistro_exterior_n_triangles[materialIdx - 1];

		glGenBuffers(1, &bistro_exterior_VBO[materialIdx]);

		glBindBuffer(GL_ARRAY_BUFFER, bistro_exterior_VBO[materialIdx]);
		glBufferData(GL_ARRAY_BUFFER, bistro_exterior_n_triangles[materialIdx] * 3 * n_bytes_per_vertex,
			bistro_exterior_vertices[materialIdx], GL_STATIC_DRAW);

		// As the geometry data exists now in graphics memory, ...
		free(bistro_exterior_vertices[materialIdx]);

		// Initialize vertex array object.
		glGenVertexArrays(1, &bistro_exterior_VAO[materialIdx]);
		glBindVertexArray(bistro_exterior_VAO[materialIdx]);

		glBindBuffer(GL_ARRAY_BUFFER, bistro_exterior_VBO[materialIdx]);
		glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(0));
		glEnableVertexAttribArray(INDEX_VERTEX_POSITION);
		glVertexAttribPointer(INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
		glEnableVertexAttribArray(INDEX_NORMAL);
		glVertexAttribPointer(INDEX_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(INDEX_TEX_COORD);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if ((materialIdx > 0) && (materialIdx % 100 == 0))
			fprintf(stdout, " * Loaded %d bistro exterior materials into graphics memory.\n", materialIdx / 100 * 100);
	}
	fprintf(stdout, " * Loaded %d bistro exterior materials into graphics memory.\n", scene.n_materials);

	// textures
	bistro_exterior_texture_names = (GLuint*)malloc(sizeof(GLuint) * scene.n_textures);
	glGenTextures(scene.n_textures, bistro_exterior_texture_names);

	for (int texId = 0; texId < scene.n_textures; texId++) {
		glBindTexture(GL_TEXTURE_2D, bistro_exterior_texture_names[texId]);

		bool bReturn = readTexImage2D_from_file(scene.texture_file_name[texId]);

		if (bReturn) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			//glGenerateMipmap(GL_TEXTURE_2D);
			flag_texture_mapping[texId] = true;
		}
		else {
			flag_texture_mapping[texId] = false;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	fprintf(stdout, " * Loaded bistro exterior textures into graphics memory.\n");

	free(bistro_exterior_vertices);
}

void bindTexture(GLuint tex, int glTextureId, int texId) {
	if (INVALID_TEX_ID != texId) {
		glActiveTexture(GL_TEXTURE0 + glTextureId);
		glBindTexture(GL_TEXTURE_2D, bistro_exterior_texture_names[texId]);
		glUniform1i(tex, glTextureId);
	}
}

void draw_bistro_exterior(void) {
	glUseProgram(h_ShaderProgram_TXPBR);
	ModelViewMatrix = ViewMatrix;
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPBR, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPBR, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPBR, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	glUniform4fv(loc_cameraPos, 1, current_camera.pos);

	for (int materialIdx = 0; materialIdx < scene.n_materials; materialIdx++) {
		int diffuseTexId = scene.material_list[materialIdx].diffuseTexId;
		int normalMapTexId = scene.material_list[materialIdx].normalMapTexId;
		int specularTexId = scene.material_list[materialIdx].specularTexId;;
		int emissiveTexId = scene.material_list[materialIdx].emissiveTexId;

		bindTexture(loc_material.diffuseTex, TEXTURE_INDEX_DIFFUSE, diffuseTexId);
		bindTexture(loc_material.normalTex, TEXTURE_INDEX_NORMAL, normalMapTexId);
		bindTexture(loc_material.specularTex, TEXTURE_INDEX_SPECULAR, specularTexId);
		bindTexture(loc_material.emissiveTex, TEXTURE_INDEX_EMISSIVE, emissiveTexId);
		glEnable(GL_TEXTURE_2D);

		glBindVertexArray(bistro_exterior_VAO[materialIdx]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * bistro_exterior_n_triangles[materialIdx]);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glUseProgram(0);
}

// skybox
GLuint skybox_VBO, skybox_VAO;
GLuint skybox_texture_name;

GLfloat cube_vertices[72][3] = {
	// vertices enumerated clockwise
	  // 6*2*3 * 2 (POS & NORM)

	// position
	-1.0f,  1.0f, -1.0f,    1.0f,  1.0f, -1.0f,    1.0f,  1.0f,  1.0f, //right
	 1.0f,  1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f, //left
	 1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f,

	-1.0f, -1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f, //top
	 1.0f,  1.0f,  1.0f,    1.0f, -1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f, //bottom
	 1.0f, -1.0f, -1.0f,    1.0f,  1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,   -1.0f, -1.0f, -1.0f,   -1.0f,  1.0f, -1.0f, //back
	-1.0f,  1.0f, -1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,    1.0f, -1.0f,  1.0f,    1.0f,  1.0f,  1.0f, //front
	 1.0f,  1.0f,  1.0f,    1.0f,  1.0f, -1.0f,    1.0f, -1.0f, -1.0f,

	 // normal
	 0.0f, 0.0f, -1.0f,      0.0f, 0.0f, -1.0f,     0.0f, 0.0f, -1.0f,
	 0.0f, 0.0f, -1.0f,      0.0f, 0.0f, -1.0f,     0.0f, 0.0f, -1.0f,

	-1.0f, 0.0f,  0.0f,     -1.0f, 0.0f,  0.0f,    -1.0f, 0.0f,  0.0f,
	-1.0f, 0.0f,  0.0f,     -1.0f, 0.0f,  0.0f,    -1.0f, 0.0f,  0.0f,

	 1.0f, 0.0f,  0.0f,      1.0f, 0.0f,  0.0f,     1.0f, 0.0f,  0.0f,
	 1.0f, 0.0f,  0.0f,      1.0f, 0.0f,  0.0f,     1.0f, 0.0f,  0.0f,

	 0.0f, 0.0f, 1.0f,      0.0f, 0.0f, 1.0f,     0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,      0.0f, 0.0f, 1.0f,     0.0f, 0.0f, 1.0f,

	 0.0f, 1.0f, 0.0f,      0.0f, 1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,      0.0f, 1.0f, 0.0f,     0.0f, 1.0f, 0.0f,

	 0.0f, -1.0f, 0.0f,      0.0f, -1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
	 0.0f, -1.0f, 0.0f,      0.0f, -1.0f, 0.0f,     0.0f, -1.0f, 0.0f
};

void readTexImage2DForCubeMap(const char* filename, GLenum texture_target) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	//fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);

	width = FreeImage_GetWidth(tx_pixmap);
	height = FreeImage_GetHeight(tx_pixmap);
	FreeImage_FlipVertical(tx_pixmap);
	data = FreeImage_GetBits(tx_pixmap);

	glTexImage2D(texture_target, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	//fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap);
}

void prepare_skybox(void) { // Draw skybox.
	glGenVertexArrays(1, &skybox_VAO);
	glGenBuffers(1, &skybox_VBO);

	glBindVertexArray(skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GLfloat), &cube_vertices[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenTextures(1, &skybox_texture_name);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_name);

	readTexImage2DForCubeMap("Scene/Cubemap/px.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	readTexImage2DForCubeMap("Scene/Cubemap/nx.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	readTexImage2DForCubeMap("Scene/Cubemap/py.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	readTexImage2DForCubeMap("Scene/Cubemap/ny.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	readTexImage2DForCubeMap("Scene/Cubemap/pz.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	readTexImage2DForCubeMap("Scene/Cubemap/nz.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	fprintf(stdout, " * Loaded cube map textures into graphics memory.\n\n");

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void draw_skybox(void) {
	glUseProgram(h_ShaderProgram_skybox);

	glUniform1i(loc_cubemap_skybox, TEXTURE_INDEX_SKYMAP);

	ModelViewMatrix = ViewMatrix * glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(20000, 20000, 20000));
	//ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(20000.0f, 20000.0f, 20000.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_SKY, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(skybox_VAO);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_INDEX_SKYMAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_name);

	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 2 * 3);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glUseProgram(0);
}

// tiger object
#define N_TIGER_FRAMES 12
GLuint tiger_VBO, tiger_VAO;
int tiger_n_triangles[N_TIGER_FRAMES];
int tiger_vertex_offset[N_TIGER_FRAMES];
GLfloat* tiger_vertices[N_TIGER_FRAMES];

void prepare_tiger_20171665(void) { // vertices enumerated clockwise
	int i, n_bytes_per_vertex, n_bytes_per_triangle, tiger_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/tiger/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		tiger_n_triangles[i] = read_geometry(&tiger_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		tiger_n_total_triangles += tiger_n_triangles[i];

		if (i == 0)
			tiger_vertex_offset[i] = 0;
		else
			tiger_vertex_offset[i] = tiger_vertex_offset[i - 1] + 3 * tiger_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &tiger_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glBufferData(GL_ARRAY_BUFFER, tiger_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_TIGER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, tiger_vertex_offset[i] * n_bytes_per_vertex,
			tiger_n_triangles[i] * n_bytes_per_triangle, tiger_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_TIGER_FRAMES; i++)
		free(tiger_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &tiger_VAO);
	glBindVertexArray(tiger_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/*material_tiger.ambient_color[0] = 0.24725f;
	material_tiger.ambient_color[1] = 0.1995f;
	material_tiger.ambient_color[2] = 0.0745f;
	material_tiger.ambient_color[3] = 1.0f;

	material_tiger.diffuse_color[0] = 0.75164f;
	material_tiger.diffuse_color[1] = 0.60648f;
	material_tiger.diffuse_color[2] = 0.22648f;
	material_tiger.diffuse_color[3] = 1.0f;

	material_tiger.specular_color[0] = 0.728281f;
	material_tiger.specular_color[1] = 0.655802f;
	material_tiger.specular_color[2] = 0.466065f;
	material_tiger.specular_color[3] = 1.0f;

	material_tiger.specular_exponent = 51.2f;

	material_tiger.emissive_color[0] = 0.1f;
	material_tiger.emissive_color[1] = 0.1f;
	material_tiger.emissive_color[2] = 0.0f;
	material_tiger.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);

	My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
}

#define TIGER_DEGREE1 60
#define TIGER_DEGREE2 30
#define TIGER_DEGREE3 60

#define TIGER_PHASE1 300
#define TIGER_PHASE2 (TIGER_PHASE1 + TIGER_DEGREE1)
#define TIGER_PHASE3 (TIGER_PHASE2 + 360)
#define TIGER_PHASE4 (TIGER_PHASE3 + TIGER_DEGREE2)
#define TIGER_PHASE5 (TIGER_PHASE4 + 120)
#define TIGER_PHASE6 (TIGER_PHASE5 + 20)
#define TIGER_PHASE7 (TIGER_PHASE6 + 80)
#define TIGER_PHASE8 (TIGER_PHASE7 + 20)
#define TIGER_PHASE9 (TIGER_PHASE8 + 120)
#define TIGER_PHASE10 (TIGER_PHASE9 + TIGER_DEGREE2)
#define TIGER_PHASE11 (TIGER_PHASE10 + 360)
#define TIGER_PHASE12 (TIGER_PHASE11 + TIGER_DEGREE1)

#define TIGER_CYCLE TIGER_PHASE12

void draw_tiger_20171665(void) {
	glUseProgram(h_ShaderProgram_simple);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);

	float radius1 = 760.0f;
	float radius2 = 380.0f;
	float radius3 = 150.0f;

	float distance1 = 3600.0f;
	float distance2 = 1440.0f;

	glm::mat4 TigerModelMatrix = glm::mat4(1.0f);
	TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(4486.0f, -2487.0f, 5.0f));
	TigerModelMatrix = glm::rotate(TigerModelMatrix, -19.8f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	
	int timestamp_tiger = timestamp_for_tiger % TIGER_CYCLE;

	float start_x1 = -radius1 * sqrt(3.0f);
	float start_y1 = 0.0f;

	float start_x2 = -(2.0f * radius2 * glm::sin(TIGER_DEGREE2 / 2.0f * TO_RADIAN) * glm::cos(TIGER_DEGREE2 / 2.0f * TO_RADIAN)) - distance1 - sqrt(3.0f) * radius1;
	float start_y2 = 2.0f * radius2 * glm::sin(TIGER_DEGREE2 / 2.0f * TO_RADIAN) * glm::sin(TIGER_DEGREE2 / 2.0f * TO_RADIAN);

	float start_x3 = start_x2 + -(distance2) * glm::cos(TIGER_DEGREE2 * TO_RADIAN);
	float start_y3 = start_y2 + distance2 * glm::sin(TIGER_DEGREE2 * TO_RADIAN);

	float center_x1 = start_x3 + radius3 * glm::sin(TIGER_DEGREE2 * TO_RADIAN);
	float center_y1 = start_y3 + radius3 * glm::cos(TIGER_DEGREE2 * TO_RADIAN);

	float center_x2 = start_x2 + -(distance2 + sqrt(3.0f) * radius3) * glm::cos(TIGER_DEGREE2 * TO_RADIAN);
	float center_y2 = start_y2 + (distance2 + sqrt(3.0f) *radius3) * glm::sin(TIGER_DEGREE2 * TO_RADIAN);

	float center_x3 = start_x3 - radius3 * glm::sin(TIGER_DEGREE2 * TO_RADIAN);
	float center_y3 = start_y3 - radius3 * glm::cos(TIGER_DEGREE2 * TO_RADIAN);

	if (timestamp_tiger < TIGER_PHASE1) {
		float time_factor = timestamp_tiger % TIGER_PHASE1;
		
		//TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(4486.0f, -2487.0f, 5.0f));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, (time_factor + 30.0f) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(-radius1, 0.0f, 0.0f));
	}
	else if (timestamp_tiger < TIGER_PHASE2) {
		float time_factor = (timestamp_tiger - TIGER_PHASE1) % (TIGER_PHASE2 - TIGER_PHASE1);
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(-radius1 * sqrt(3.0f), radius1, 0.0f));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, -(time_factor + 30.0f) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(radius1, 0.0f, 0.0f));
	}
	else if (timestamp_tiger < TIGER_PHASE3) {
		float time_factor = (timestamp_tiger - TIGER_PHASE2) % (TIGER_PHASE3 - TIGER_PHASE2);
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(
			start_x1 + (-distance1) / (TIGER_PHASE3 - TIGER_PHASE2) * time_factor,
            0.0f, 0.0f
		));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (timestamp_tiger < TIGER_PHASE4) {
		float time_factor = (timestamp_tiger - TIGER_PHASE3) % (TIGER_PHASE4 - TIGER_PHASE3);

		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(-distance1 - sqrt(3.0f) * radius1, radius2, 0.0f));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, (90.0f - time_factor) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(-radius2, 0.0f, 0.0f));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (timestamp_tiger < TIGER_PHASE5) {
		float time_factor = (timestamp_tiger - TIGER_PHASE4) % (TIGER_PHASE5 - TIGER_PHASE4);
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(
			start_x2 + -(distance2 * glm::cos(TIGER_DEGREE2 * TO_RADIAN)) / (TIGER_PHASE5 - TIGER_PHASE4) * time_factor,
			start_y2 + (distance2 * glm::sin(TIGER_DEGREE2 * TO_RADIAN)) / (TIGER_PHASE5 - TIGER_PHASE4) * time_factor,
			0.0f
		));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, (-90.0f - TIGER_DEGREE2) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (timestamp_tiger < TIGER_PHASE6) {
		float time_factor = (timestamp_tiger - TIGER_PHASE5) % (TIGER_PHASE6 - TIGER_PHASE5) * (60.0f / (TIGER_PHASE6 - TIGER_PHASE5));
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(center_x1, center_y1, 0.0f));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, (90.0f - TIGER_DEGREE2 - time_factor) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(-radius3, 0.0f, 0.0f));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (timestamp_tiger < TIGER_PHASE7) {
		float time_factor = (timestamp_tiger - TIGER_PHASE6) % (TIGER_PHASE7 - TIGER_PHASE6) * (300.0f / (TIGER_PHASE7 - TIGER_PHASE6));

		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(center_x2, center_y2, 0.0f));
		TigerModelMatrix = glm::rotate(TigerModelMatrix,
			(180.0f - TIGER_DEGREE2 + 30.0f +  time_factor) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f)
		);
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(-radius3, 0.0f, 0.0f));
	}
	else if (timestamp_tiger < TIGER_PHASE8) {
		float time_factor = (timestamp_tiger - TIGER_PHASE7) % (TIGER_PHASE8 - TIGER_PHASE7) * (60.0f / (TIGER_PHASE8 - TIGER_PHASE7));
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(center_x3, center_y3, 0.0f));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, (-90.0f - TIGER_DEGREE2 + 60.0f - time_factor) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(-radius3, 0.0f, 0.0f));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (timestamp_tiger < TIGER_PHASE9) {
		float time_factor = (timestamp_tiger - TIGER_PHASE8) % (TIGER_PHASE9 - TIGER_PHASE8);
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(
			start_x3 + (distance2 * glm::cos(TIGER_DEGREE2 * TO_RADIAN)) / (TIGER_PHASE9 - TIGER_PHASE8) * time_factor,
			start_y3 + -(distance2 * glm::sin(TIGER_DEGREE2 * TO_RADIAN)) / (TIGER_PHASE9 - TIGER_PHASE8) * time_factor,
			0.0f
		));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, (-90.0f - TIGER_DEGREE2 + 180.0f) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (timestamp_tiger < TIGER_PHASE10) {
		float time_factor = (timestamp_tiger - TIGER_PHASE9) % (TIGER_PHASE10 - TIGER_PHASE9);

		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(-distance1 - sqrt(3.0f) * radius1, radius2, 0.0f));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, (90.0f - TIGER_DEGREE2 + time_factor) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(-radius2, 0.0f, 0.0f));
	}
	else if (timestamp_tiger < TIGER_PHASE11) {
		float time_factor = (timestamp_tiger - TIGER_PHASE10) % (TIGER_PHASE11 - TIGER_PHASE10);
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(
			-distance1 - sqrt(3.0f) * radius1 + (distance1) / (TIGER_PHASE11 - TIGER_PHASE10) * time_factor,
			0.0f, 0.0f
		));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (timestamp_tiger < TIGER_PHASE12) {
		float time_factor = (timestamp_tiger - TIGER_PHASE11) % (TIGER_PHASE12 - TIGER_PHASE11);
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(-radius1 * sqrt(3.0f), -radius1, 0.0f));
		TigerModelMatrix = glm::rotate(TigerModelMatrix, (90.0f - time_factor) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		TigerModelMatrix = glm::translate(TigerModelMatrix, glm::vec3(radius1, 0.0f, 0.0f));
	}
	
	TigerModelMatrix = glm::scale(TigerModelMatrix, glm::vec3(1.5f, 1.5f, 1.5f));
	TigerModelViewMatrix = TigerModelMatrix;
	ModelViewMatrix = ViewMatrix * TigerModelMatrix;
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(tiger_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_TRIANGLES, tiger_vertex_offset[cur_frame_tiger], 3 * tiger_n_triangles[cur_frame_tiger]);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(0);
}



// spider object
#define N_SPIDER_FRAMES 16
GLuint spider_VBO, spider_VAO;
int spider_n_triangles[N_SPIDER_FRAMES];
int spider_vertex_offset[N_SPIDER_FRAMES];
GLfloat* spider_vertices[N_SPIDER_FRAMES];


void prepare_spider_20171665(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, spider_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_SPIDER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/spider/spider_vnt_%d%d.geom", i / 10, i % 10);
		spider_n_triangles[i] = read_geometry(&spider_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		spider_n_total_triangles += spider_n_triangles[i];

		if (i == 0)
			spider_vertex_offset[i] = 0;
		else
			spider_vertex_offset[i] = spider_vertex_offset[i - 1] + 3 * spider_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &spider_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glBufferData(GL_ARRAY_BUFFER, spider_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_SPIDER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, spider_vertex_offset[i] * n_bytes_per_vertex,
			spider_n_triangles[i] * n_bytes_per_triangle, spider_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_SPIDER_FRAMES; i++)
		free(spider_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &spider_VAO);
	glBindVertexArray(spider_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_spider.ambient_color[0] = 0.24725f;
	//material_spider.ambient_color[1] = 0.1995f;
	//material_spider.ambient_color[2] = 0.0745f;
	//material_spider.ambient_color[3] = 1.0f;
	//
	//material_spider.diffuse_color[0] = 0.75164f;
	//material_spider.diffuse_color[1] = 0.60648f;
	//material_spider.diffuse_color[2] = 0.22648f;
	//material_spider.diffuse_color[3] = 1.0f;
	//
	//material_spider.specular_color[0] = 0.728281f;
	//material_spider.specular_color[1] = 0.655802f;
	//material_spider.specular_color[2] = 0.466065f;
	//material_spider.specular_color[3] = 1.0f;
	//
	//material_spider.specular_exponent = 51.2f;
	//
	//material_spider.emissive_color[0] = 0.1f;
	//material_spider.emissive_color[1] = 0.1f;
	//material_spider.emissive_color[2] = 0.0f;
	//material_spider.emissive_color[3] = 1.0f;

	//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	//glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	//glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);

	//My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");

	//glGenerateMipmap(GL_TEXTURE_2D);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


void draw_spider_20171665(void) {
	glUseProgram(h_ShaderProgram_simple);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);

	float time_factor = timestamp_scene % 360;

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-816.0f, -774.0f, 611.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f * TO_RADIAN - glm::atan(9.0f / 10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -270.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(
		200.0f * glm::cos(time_factor * TO_RADIAN) * glm::cos(2.0f * time_factor * TO_RADIAN),
		200.0f * glm::cos(time_factor * TO_RADIAN) * glm::sin(2.0f * time_factor * TO_RADIAN),
		0.0f
	));

	ModelViewMatrix = glm::rotate(ModelViewMatrix,
		abs(atan((-200.0f * sin(time_factor * TO_RADIAN) * sin(time_factor * 2.0f * TO_RADIAN) * TO_RADIAN + 400.0f * cos(time_factor * TO_RADIAN) * cos(time_factor * 2.0f * TO_RADIAN) * TO_RADIAN) /
			(-200.0f * sin(time_factor * TO_RADIAN) * cos(time_factor * 2.0f * TO_RADIAN) * TO_RADIAN - 400.0f * cos(time_factor * TO_RADIAN) * sin(time_factor * 2.0f * TO_RADIAN) * TO_RADIAN))),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(spider_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_TRIANGLES, spider_vertex_offset[cur_frame_spider], 3 * spider_n_triangles[cur_frame_spider]);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(0);
}


// wolf object
#define N_WOLF_FRAMES 17
GLuint wolf_VBO, wolf_VAO;
int wolf_n_triangles[N_WOLF_FRAMES];
int wolf_vertex_offset[N_WOLF_FRAMES];
GLfloat* wolf_vertices[N_WOLF_FRAMES];

#define WOLF_CYCLE 360
#define WOLF_PHASE1 140
#define WOLF_PHASE2 180
#define WOLF_PHASE3 320
#define WOLF_PHASE4 360

void prepare_wolf_20171665(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, wolf_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_WOLF_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/wolf/wolf_%02d_vnt.geom", i);
		wolf_n_triangles[i] = read_geometry(&wolf_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		wolf_n_total_triangles += wolf_n_triangles[i];

		if (i == 0)
			wolf_vertex_offset[i] = 0;
		else
			wolf_vertex_offset[i] = wolf_vertex_offset[i - 1] + 3 * wolf_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &wolf_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glBufferData(GL_ARRAY_BUFFER, wolf_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_WOLF_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, wolf_vertex_offset[i] * n_bytes_per_vertex,
			wolf_n_triangles[i] * n_bytes_per_triangle, wolf_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_WOLF_FRAMES; i++)
		free(wolf_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &wolf_VAO);
	glBindVertexArray(wolf_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_wolf.ambient_color[0] = 0.24725f;
	//material_wolf.ambient_color[1] = 0.1995f;
	//material_wolf.ambient_color[2] = 0.0745f;
	//material_wolf.ambient_color[3] = 1.0f;
	
	//material_wolf.diffuse_color[0] = 0.75164f;
	//material_wolf.diffuse_color[1] = 0.60648f;
	//material_wolf.diffuse_color[2] = 0.22648f;
	//material_wolf.diffuse_color[3] = 1.0f;
	
	//material_wolf.specular_color[0] = 0.728281f;
	//material_wolf.specular_color[1] = 0.655802f;
	//material_wolf.specular_color[2] = 0.466065f;
	//material_wolf.specular_color[3] = 1.0f;
	
	//material_wolf.specular_exponent = 51.2f;
	
	//material_wolf.emissive_color[0] = 0.1f;
	//material_wolf.emissive_color[1] = 0.1f;
	//material_wolf.emissive_color[2] = 0.0f;
	//material_wolf.emissive_color[3] = 1.0f;

	//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	//glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	//glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);

	//My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");

	//glGenerateMipmap(GL_TEXTURE_2D);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void draw_wolf_20171665(void) {
	glUseProgram(h_ShaderProgram_simple);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);

	float time_factor_xy = timestamp_scene % WOLF_CYCLE;
	float time_factor_z = timestamp_scene % 36;
	float height = 0.0f;
	float cx = 1800.0f;
	float radius = 300.0f;

	float intersect[4][2];

	if (sin(time_factor_z * TO_RADIAN) > 0) {
		height = sin(time_factor_z * 10.0f * TO_RADIAN) * 50.0f;
	}
	else {
		height = 0.0f;
	}

	float alpha = glm::acos(radius / cx);
	float ix = cx - radius * glm::cos(alpha);
	float iy = radius * glm::sin(alpha);

	intersect[0][0] = ix;
	intersect[0][1] = iy;

	intersect[1][0] = ix;
	intersect[1][1] = -iy;

	intersect[2][0] = -ix;
	intersect[2][1] = iy;

	intersect[3][0] = -ix;
	intersect[3][1] = -iy;


	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(462.0f, 2082.0f, 20.0f + height));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 55.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	
	if (time_factor_xy < WOLF_PHASE1) {
		float nx = intersect[3][0] + (intersect[0][0] - intersect[3][0]) / WOLF_PHASE1 * time_factor_xy;
		float ny = intersect[3][1] + (intersect[0][1] - intersect[3][1]) / WOLF_PHASE1 * time_factor_xy;
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(nx, ny, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN - alpha, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (time_factor_xy < WOLF_PHASE2) {
		time_factor_xy = ((int)time_factor_xy - WOLF_PHASE1) % (WOLF_PHASE2 - WOLF_PHASE1);
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(cx, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix,
			-(360.0f * TO_RADIAN - 2.0f * alpha) / (WOLF_PHASE2 - WOLF_PHASE1) * (time_factor_xy),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(intersect[0][0] - cx, intersect[0][1], 0.0f));
	}
	else if (time_factor_xy < WOLF_PHASE3) {
		time_factor_xy = ((int)time_factor_xy - WOLF_PHASE2) % (WOLF_PHASE3 - WOLF_PHASE2);
		float nx = intersect[1][0] + (intersect[2][0] - intersect[1][0]) / (WOLF_PHASE3 - WOLF_PHASE2) * time_factor_xy;
		float ny = intersect[1][1] + (intersect[2][1] - intersect[1][1]) / (WOLF_PHASE3 - WOLF_PHASE2) * time_factor_xy;
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(nx, ny, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN + alpha, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (time_factor_xy < WOLF_PHASE4) {
		time_factor_xy = ((int)time_factor_xy - WOLF_PHASE3) % (WOLF_PHASE4 - WOLF_PHASE3);
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(-cx, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix,
			(360.0f * TO_RADIAN - 2.0f * alpha) / (WOLF_PHASE4 - WOLF_PHASE3) * (time_factor_xy),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(intersect[2][0] + cx, intersect[2][1], 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(400.0f, 400.0f, 400.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(wolf_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_TRIANGLES, wolf_vertex_offset[cur_frame_wolf], 3 * wolf_n_triangles[cur_frame_wolf]);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(0);
}

// bus object
GLuint bus_VBO, bus_VAO;
int bus_n_triangles;
GLfloat* bus_vertices;

void prepare_bus_20171665(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, bus_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/bus_vnt.geom");
	bus_n_triangles = read_geometry(&bus_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	bus_n_total_triangles += bus_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &bus_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glBufferData(GL_ARRAY_BUFFER, bus_n_total_triangles * 3 * n_bytes_per_vertex, bus_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(bus_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &bus_VAO);
	glBindVertexArray(bus_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_bus.ambient_color[0] = 0.24725f;
	//material_bus.ambient_color[1] = 0.1995f;
	//material_bus.ambient_color[2] = 0.0745f;
	//material_bus.ambient_color[3] = 1.0f;
	//
	//material_bus.diffuse_color[0] = 0.75164f;
	//material_bus.diffuse_color[1] = 0.60648f;
	//material_bus.diffuse_color[2] = 0.22648f;
	//material_bus.diffuse_color[3] = 1.0f;
	//
	//material_bus.specular_color[0] = 0.728281f;
	//material_bus.specular_color[1] = 0.655802f;
	//material_bus.specular_color[2] = 0.466065f;
	//material_bus.specular_color[3] = 1.0f;
	//
	//material_bus.specular_exponent = 51.2f;
	//
	//material_bus.emissive_color[0] = 0.1f;
	//material_bus.emissive_color[1] = 0.1f;
	//material_bus.emissive_color[2] = 0.0f;
	//material_bus.emissive_color[3] = 1.0f;

	/*glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);

	My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);*/

	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
}


void draw_bus_20171665(void) {
	glUseProgram(h_ShaderProgram_simple);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(4798.0f, -1322.0f, 20.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 95.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(40.0f, 40.0f, 40.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(bus_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * bus_n_triangles);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(0);
}


// optimus object
GLuint optimus_VBO, optimus_VAO;
int optimus_n_triangles;
GLfloat* optimus_vertices;

void prepare_optimus_20171665(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, optimus_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/optimus_vnt.geom");
	optimus_n_triangles = read_geometry(&optimus_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	optimus_n_total_triangles += optimus_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &optimus_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, optimus_VBO);
	glBufferData(GL_ARRAY_BUFFER, optimus_n_total_triangles * 3 * n_bytes_per_vertex, optimus_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(optimus_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &optimus_VAO);
	glBindVertexArray(optimus_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, optimus_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_optimus.ambient_color[0] = 0.24725f;
	//material_optimus.ambient_color[1] = 0.1995f;
	//material_optimus.ambient_color[2] = 0.0745f;
	//material_optimus.ambient_color[3] = 1.0f;
	//
	//material_optimus.diffuse_color[0] = 0.75164f;
	//material_optimus.diffuse_color[1] = 0.60648f;
	//material_optimus.diffuse_color[2] = 0.22648f;
	//material_optimus.diffuse_color[3] = 1.0f;
	//
	//material_optimus.specular_color[0] = 0.728281f;
	//material_optimus.specular_color[1] = 0.655802f;
	//material_optimus.specular_color[2] = 0.466065f;
	//material_optimus.specular_color[3] = 1.0f;
	//
	//material_optimus.specular_exponent = 51.2f;
	//
	//material_optimus.emissive_color[0] = 0.1f;
	//material_optimus.emissive_color[1] = 0.1f;
	//material_optimus.emissive_color[2] = 0.0f;
	//material_optimus.emissive_color[3] = 1.0f;

	/*glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);

	My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
}

void draw_optimus_20171665(void) {
	glUseProgram(h_ShaderProgram_simple);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-1750.0f, 905.0f, 10.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -30.0f  *TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(optimus_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * optimus_n_triangles);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(0);
}


// bike object
GLuint bike_VBO, bike_VAO;
int bike_n_triangles;
GLfloat* bike_vertices;


void prepare_bike_20171665(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, bike_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/bike_vnt.geom");
	bike_n_triangles = read_geometry(&bike_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	bike_n_total_triangles += bike_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &bike_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, bike_VBO);
	glBufferData(GL_ARRAY_BUFFER, bike_n_total_triangles * 3 * n_bytes_per_vertex, bike_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(bike_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &bike_VAO);
	glBindVertexArray(bike_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, bike_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_bike.ambient_color[0] = 0.24725f;
	//material_bike.ambient_color[1] = 0.1995f;
	//material_bike.ambient_color[2] = 0.0745f;
	//material_bike.ambient_color[3] = 1.0f;
	//
	//material_bike.diffuse_color[0] = 0.75164f;
	//material_bike.diffuse_color[1] = 0.60648f;
	//material_bike.diffuse_color[2] = 0.22648f;
	//material_bike.diffuse_color[3] = 1.0f;
	//
	//material_bike.specular_color[0] = 0.728281f;
	//material_bike.specular_color[1] = 0.655802f;
	//material_bike.specular_color[2] = 0.466065f;
	//material_bike.specular_color[3] = 1.0f;
	//
	//material_bike.specular_exponent = 51.2f;
	//
	//material_bike.emissive_color[0] = 0.1f;
	//material_bike.emissive_color[1] = 0.1f;
	//material_bike.emissive_color[2] = 0.0f;
	//material_bike.emissive_color[3] = 1.0f;

	//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	//glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	//glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);

	//My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");

	//glGenerateMipmap(GL_TEXTURE_2D);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void draw_bike_20171665(void) {
	glUseProgram(h_ShaderProgram_simple);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-1563.0f, 173.0f, 20.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -135.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(60.0f, 60.0f, 60.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	
	glBindVertexArray(bike_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * bike_n_triangles);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(0);
}


// dragon object
GLuint dragon_VBO, dragon_VAO;
int dragon_n_triangles;
GLfloat* dragon_vertices;

Material_Parameters material_dragon;


void prepare_dragon_20171665(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, dragon_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/dragon_vnt.geom");
	dragon_n_triangles = read_geometry(&dragon_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	dragon_n_total_triangles += dragon_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &dragon_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, dragon_VBO);
	glBufferData(GL_ARRAY_BUFFER, dragon_n_total_triangles * 3 * n_bytes_per_vertex, dragon_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(dragon_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &dragon_VAO);
	glBindVertexArray(dragon_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, dragon_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_dragon.ambient_color[0] = 0.24725f;
	//material_dragon.ambient_color[1] = 0.1995f;
	//material_dragon.ambient_color[2] = 0.0745f;
	//material_dragon.ambient_color[3] = 1.0f;
	//
	//material_dragon.diffuse_color[0] = 0.75164f;
	//material_dragon.diffuse_color[1] = 0.60648f;
	//material_dragon.diffuse_color[2] = 0.22648f;
	//material_dragon.diffuse_color[3] = 1.0f;
	//
	//material_dragon.specular_color[0] = 0.728281f;
	//material_dragon.specular_color[1] = 0.655802f;
	//material_dragon.specular_color[2] = 0.466065f;
	//material_dragon.specular_color[3] = 1.0f;
	//
	//material_dragon.specular_exponent = 51.2f;
	//
	//material_dragon.emissive_color[0] = 0.1f;
	//material_dragon.emissive_color[1] = 0.1f;
	//material_dragon.emissive_color[2] = 0.0f;
	//material_dragon.emissive_color[3] = 1.0f;

	/*glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);

	My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
}

void draw_dragon_20171665(void) {
	glUseProgram(h_ShaderProgram_simple);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(1273.0f, 3202.0f, 20.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -120.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(20.0f, 20.0f, 20.0f));

	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);


	glBindVertexArray(dragon_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * dragon_n_triangles);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(0);
}


// godzilla object
GLuint godzilla_VBO, godzilla_VAO;
int godzilla_n_triangles;
GLfloat* godzilla_vertices;


void prepare_godzilla_20171665(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, godzilla_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/godzilla_vnt.geom");
	godzilla_n_triangles = read_geometry(&godzilla_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	godzilla_n_total_triangles += godzilla_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &godzilla_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, godzilla_VBO);
	glBufferData(GL_ARRAY_BUFFER, godzilla_n_total_triangles * 3 * n_bytes_per_vertex, godzilla_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(godzilla_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &godzilla_VAO);
	glBindVertexArray(godzilla_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, godzilla_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_godzilla.ambient_color[0] = 0.24725f;
	//material_godzilla.ambient_color[1] = 0.1995f;
	//material_godzilla.ambient_color[2] = 0.0745f;
	//material_godzilla.ambient_color[3] = 1.0f;
	//
	//material_godzilla.diffuse_color[0] = 0.75164f;
	//material_godzilla.diffuse_color[1] = 0.60648f;
	//material_godzilla.diffuse_color[2] = 0.22648f;
	//material_godzilla.diffuse_color[3] = 1.0f;
	//
	//material_godzilla.specular_color[0] = 0.728281f;
	//material_godzilla.specular_color[1] = 0.655802f;
	//material_godzilla.specular_color[2] = 0.466065f;
	//material_godzilla.specular_color[3] = 1.0f;
	//
	//material_godzilla.specular_exponent = 51.2f;
	//
	//material_godzilla.emissive_color[0] = 0.1f;
	//material_godzilla.emissive_color[1] = 0.1f;
	//material_godzilla.emissive_color[2] = 0.0f;
	//material_godzilla.emissive_color[3] = 1.0f;

	/*glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);

	My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex2.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
}

void draw_godzilla_20171665(void) {
	glUseProgram(h_ShaderProgram_simple);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(4760.0f, -3970.0f, 25.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -135.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(6.0f, 6.0f, 6.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(godzilla_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * godzilla_n_triangles);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(0);
}


// sphere object
GLuint sphere_VBO, sphere_VAO;
const int num_sphere_latitude = 24;
const int num_sphere_longitude = 12;
const float radius = 60.0f;
const int num_sphere_vertices = num_sphere_latitude * num_sphere_longitude;

GLfloat sphere_vertices[num_sphere_vertices][3];

void set_sphere_vertices() {
    float theta_delta = 2.0f * PI / num_sphere_latitude;
    float phi_delta = PI / num_sphere_longitude;

	for (int i = 0; i < num_sphere_latitude; i++) {
        float theta = i * theta_delta;
		for (int j = 0; j < num_sphere_longitude; j++) {
			float phi = j * phi_delta - (PI / 2);
			
            int k = i * num_sphere_longitude + j;
            float x = cos(theta) * cos(phi);
            float y = sin(theta) * cos(phi);
            float z = sin(phi);

            sphere_vertices[k][0] = radius * x;
            sphere_vertices[k][1] = radius * y;
            sphere_vertices[k][2] = radius * z;
        }
    }
}

void prepare_sphere_20171665(void) {
	set_sphere_vertices();
	// Initialize vertex buffer object.
	glGenBuffers(1, &sphere_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, sphere_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), &sphere_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &sphere_VAO);
	glBindVertexArray(sphere_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, sphere_VAO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sphere_20171665(int x, int y, float distance) {
	glUseProgram(h_ShaderProgram_simple);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CW);

	glm::mat3 ViewMatrixInverse = glm::mat3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2],
		current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2],
		current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]
	);

	float near_clip_height = 2.0f * current_camera.near_c * glm::tan(current_camera.fovy / 2.0f);
	float near_clip_width = near_clip_height * current_camera.aspect_ratio;
	float relative_x = (x - window_width / 2.0f) / window_width * near_clip_width;
	float relative_y = -(y - window_height / 2.0f) / window_height * near_clip_height;
	glm::vec3 clickDirection = ViewMatrixInverse * glm::vec3(relative_x, relative_y, -current_camera.near_c);
	clickDirection = glm::normalize(clickDirection);

	if (!left_mouse_pressed) {
		sphere_position.x = current_camera.pos[0] + clickDirection.x * (distance + 1);
		sphere_position.y = current_camera.pos[1] + clickDirection.y * (distance + 1);
		sphere_position.z = current_camera.pos[2] + clickDirection.z * (distance + 1);
	}

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(sphere_position.x, sphere_position.y, sphere_position.z));
	//ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(sphere_VAO);
	glUniform3fv(loc_primitive_color, 1, grid_color);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, num_sphere_vertices);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(0);
}

/*****************************  END: geometry setup *****************************/

/********************  START: callback function definitions *********************/


void timer_scene(int value) {
	timestamp_scene = (timestamp_scene + 1) % UINT_MAX;
	if (flag_animation) {
		timestamp_for_tiger = (timestamp_for_tiger + 1) % UINT_MAX;
		cur_frame_tiger = timestamp_for_tiger % N_TIGER_FRAMES;
	}
	/*cur_frame_ben = timestamp_scene % N_BEN_FRAMES;*/
	cur_frame_wolf = timestamp_scene % N_WOLF_FRAMES;
	cur_frame_spider = timestamp_scene % N_SPIDER_FRAMES;
	rotation_angle_tiger = (timestamp_scene % 360) * TO_RADIAN;
	glutPostRedisplay();
	glutTimerFunc(10, timer_scene, 0);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_grid();
	draw_axes();
	draw_bistro_exterior();
	draw_skybox();
	
	
	// dynamic object
	draw_tiger_20171665();
	draw_spider_20171665();
	draw_wolf_20171665();
	
	// static object
	draw_bus_20171665();
	draw_optimus_20171665();
	draw_bike_20171665();
	draw_dragon_20171665();
	draw_godzilla_20171665();

	if (z_pressed) {
		draw_sphere_20171665(z_x, z_y, z_distance);
	}

	if (tiger_view_active) {
		current_camera_num = CAMERA_TIGER;
		set_current_camera(current_camera_num);
		update_view_projection_matrix_20171665();
		//glutPostRedisplay();
	}
	else if (tiger_rear_view_active) {
		current_camera_num = CAMERA_TIGER_REAR;
		set_current_camera(current_camera_num);
		update_view_projection_matrix_20171665();
		//glutPostRedisplay();
	}
	
	glutSwapBuffers();
}

void special(int key, int x, int y) {
	if (current_camera.move && (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT || 
		key == GLUT_KEY_UP || key == GLUT_KEY_DOWN)) {
		if (key == GLUT_KEY_LEFT) {
			update_camera_position_20171665(X_AXIS, -1.0f);
		}
		if (key == GLUT_KEY_RIGHT) {
			update_camera_position_20171665(X_AXIS, 1.0f);
		}
		if (key == GLUT_KEY_UP) {
			if (space_pressed) {
				update_camera_position_20171665(Y_AXIS, 1.0f);
			}
			else {
				update_camera_position_20171665(Z_AXIS, -1.0f);
			}
		}
		if (key == GLUT_KEY_DOWN) {
			if (space_pressed) {
				update_camera_position_20171665(Y_AXIS, -1.0f);
			}
			else {
				update_camera_position_20171665(Z_AXIS, 1.0f);
			}
		}

		set_view_matrix_from_camera_frame();
		update_view_projection_matrix_20171665();
		glutPostRedisplay();
		//print_camera_information_20171665();
	}

}

void mousepassivemove(int x, int y) {
	if (z_pressed) {
		z_x = x;
		z_y = y;
		glutPostRedisplay();
	}
}

void mousemove(int x, int y) {
	if (tiger_view_active || tiger_rear_view_active) {
		return;
	}

	if (current_camera.move) {
		if (left_mouse_pressed) {
			if (z_pressed) {
				rotate_camera_by_sphere(prev_x - x, prev_y - y);
			}
			else {
				update_camera_orientation_20171665(X_AXIS, prev_y - y);
				update_camera_orientation_20171665(Y_AXIS, prev_x - x);
			}
		}

		if (right_mouse_pressed){
			update_camera_orientation_20171665(Z_AXIS, prev_x - x);
		}

		set_view_matrix_from_camera_frame();
		update_view_projection_matrix_20171665();
		//print_camera_information_20171665();
	}
	prev_x = x;
	prev_y = y;
	z_x = x;
	z_y = y;
	glutPostRedisplay();
	
}

void mousepress(int button, int state, int x, int y) {
	if (tiger_view_active || tiger_rear_view_active) {
		return;
	}

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			left_mouse_pressed = true;
			prev_x = x;
			prev_y = y;
			if (z_pressed) {
				rotate_camera_to_point(x, y);
				update_view_projection_matrix_20171665();
				z_x = window_width / 2.0f, z_y = window_height / 2.0f;
				glutPostRedisplay();
				return;
			}
		}
        else
            left_mouse_pressed = false;
    }
	else if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			right_mouse_pressed = true;
			prev_x = x;
			prev_y = y;
		}
        else
            right_mouse_pressed = false;
    }
	else if (button == GLUT_MIDDLE_BUTTON) {
        if (state == GLUT_DOWN)
            middle_mouse_pressed = true;
        else
            middle_mouse_pressed = false;
    }
	else if (button == GLUT_SCROLL_UP) {
		if (z_pressed) {
			if ((z_distance + 20.0f) < MAX_Z_DISTANCE && !left_mouse_pressed){
				z_distance += 20.0f;
			}
		}
		else {
			int mod = glutGetModifiers();
			if (mod == GLUT_ACTIVE_CTRL) {
				if (current_camera.fovy - TO_RADIAN > 0.0f) {
					current_camera.fovy -= TO_RADIAN;
				}
			}
			update_view_projection_matrix_20171665();
			glutPostRedisplay();
		}
	}
	else if (button == GLUT_SCROLL_DOWN) {
		if (z_pressed){
			if ((z_distance - 20.0f) > MIN_Z_DISTANCE && !left_mouse_pressed) {
				z_distance -= 20.0f;
			}
		}
		else {
			int mod = glutGetModifiers();
			if (mod == GLUT_ACTIVE_CTRL) {
				if (current_camera.fovy + TO_RADIAN < 120.0f * TO_RADIAN) {
					current_camera.fovy += TO_RADIAN;
				}
			}
			update_view_projection_matrix_20171665();
			glutPostRedisplay();
		}
	}

    prev_x = x;
    prev_y = y;
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'e': // toggle the animation effect.
		flag_animation = 1 - flag_animation;
		if (flag_animation) {
			//glutTimerFunc(100, timer_scene, 0);
			fprintf(stdout, "^^^ Animation mode ON.\n");
		}
		else
			fprintf(stdout, "^^^ Animation mode OFF.\n");
		break;
	case 'f':
		b_draw_grid = b_draw_grid ? false : true;
		break;
	case 'a':
		tiger_view_active = 0;
		tiger_rear_view_active = 0;
		current_camera_num = CAMERA_6;
		set_current_camera(current_camera_num);
		current_camera.move = 1;
		update_view_projection_matrix_20171665();
		glutPostRedisplay();
		break;
	case 'u':
		tiger_view_active = 0;
		tiger_rear_view_active = 0;
		current_camera_num = CAMERA_2;
		set_current_camera(current_camera_num);
		current_camera.move = 0;
		update_view_projection_matrix_20171665();
		glutPostRedisplay();
		break;
	case 'i':
		tiger_view_active = 0;
		tiger_rear_view_active = 0;
		current_camera_num = CAMERA_3;
		set_current_camera(current_camera_num);
		current_camera.move = 0;
		update_view_projection_matrix_20171665();
		glutPostRedisplay();
		break;
	case 'o':
		tiger_view_active = 0;
		tiger_rear_view_active = 0;
		current_camera_num = CAMERA_4;
		set_current_camera(current_camera_num);
		current_camera.move = 0;
		update_view_projection_matrix_20171665();
		glutPostRedisplay();
		break;
	case 'p':
		tiger_view_active = 0;
		tiger_rear_view_active = 0;
		current_camera_num = CAMERA_5;
		set_current_camera(current_camera_num);
		current_camera.move = 0;
		update_view_projection_matrix_20171665();
		glutPostRedisplay();
		break;
	case 't':
		tiger_view_active = 1;
		tiger_rear_view_active = 0;
		break;
	case 'g':
		tiger_rear_view_active = 1;
		tiger_view_active = 0;
		break;
	case 'z':
		z_pressed = true;
		break;
	case ' ':
		space_pressed = true;
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

void keyboardup(unsigned char key, int x, int y) {
	switch (key) {
	case 'z':
		z_pressed = false;
		break;
	case ' ':
		space_pressed = false;
		break;
	}
}

void reshape(int width, int height) {
	float aspect_ratio;

	glViewport(0, 0, width, height);

	window_width = width;
	window_height = height;

	current_camera.aspect_ratio = (float)window_width / window_height;
	
	update_view_projection_matrix_20171665();

	//ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
	//ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(1, &grid_VAO);
	glDeleteBuffers(1, &grid_VBO);

	glDeleteVertexArrays(scene.n_materials, bistro_exterior_VAO);
	glDeleteBuffers(scene.n_materials, bistro_exterior_VBO);
	glDeleteTextures(scene.n_textures, bistro_exterior_texture_names);

	glDeleteVertexArrays(1, &skybox_VAO);
	glDeleteBuffers(1, &skybox_VBO);

	free(bistro_exterior_n_triangles);
	free(bistro_exterior_vertex_offset);

	free(bistro_exterior_VAO);
	free(bistro_exterior_VBO);

	free(bistro_exterior_texture_names);
	free(flag_texture_mapping);
}
/*********************  END: callback function definitions **********************/

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutSpecialFunc(special);
	glutMotionFunc(mousemove);
	glutPassiveMotionFunc(mousemove);
	glutMouseFunc(mousepress);
	glutReshapeFunc(reshape);
	glutCloseFunc(cleanup);
	glutTimerFunc(100, timer_scene, 0);
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix = glm::mat4(1.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	initialize_lights();
}

void prepare_scene(void) {
	prepare_axes();
	prepare_grid();
	prepare_bistro_exterior();
	prepare_skybox();

	prepare_tiger_20171665();
	prepare_spider_20171665();
	prepare_wolf_20171665();
	
	prepare_bus_20171665();
	prepare_bike_20171665();
	prepare_optimus_20171665();
	prepare_dragon_20171665();
	prepare_godzilla_20171665();

	prepare_sphere_20171665();
}

void initialize_flags(void) {
	flag_animation = 1;
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
	initialize_camera();
	initialize_flags();
}


void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "********************************************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "********************************************************************************\n\n");
}

void print_message(const char* m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "********************************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n********************************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 9
void drawScene(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 Bistro Exterior Scene";
	char messages[N_MESSAGE_LINES][256] = { 
		"    - Keys used:",
		"		'f' : draw x, y, z axes and grid",
		"		'1' : set the camera for original view",
		"		'2' : set the camera for bistro view",
		"		'3' : set the camera for tree view",	
		"		'4' : set the camera for top view",
		"		'5' : set the camera for front view",
		"		'6' : set the camera for side view",
		"		'ESC' : program close",
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(900, 600);
	glutInitWindowPosition(20, 20);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
