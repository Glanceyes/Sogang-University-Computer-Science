//
//  DrawScene.cpp
//
//  Written for CSE4170
//  Department of Computer Science and Engineering
//  Copyright © 2022 Sogang University. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "LoadScene.h"

// Begin of shader setup
#include "Shaders/LoadShaders.h"
#include "ShadingInfo.h"

extern SCENE scene;

// for simple shaders
GLuint h_ShaderProgram_simple, h_ShaderProgram_TXPS, h_ShaderProgram_GS; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// for Phong Shading (Textured) shaders
#define NUMBER_OF_LIGHT_SUPPORTED 10
GLint loc_global_ambient_color, loc_global_ambient_color_GS;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED], loc_light_GS[NUMBER_OF_LIGHT_SUPPORTED];
GLint loc_flag_diffuse_texture_mapping, loc_flag_normal_texture_mapping, loc_flag_emissive_texture_mapping, loc_flag_normal_based_directX, loc_flag_blending;
loc_Material_Parameters loc_material, loc_material_GS;
GLint loc_ModelViewProjectionMatrix_TXPS, loc_ModelViewMatrix_TXPS, loc_ModelViewMatrixInvTrans_TXPS;
GLint loc_ModelViewProjectionMatrix_GS, loc_ModelViewMatrix_GS, loc_ModelViewMatrixInvTrans_GS;
GLint loc_flag_fog; int flag_fog;
GLuint loc_timestamp;
GLfloat loc_fragment_alpha;

GLuint loc_flag_light_effect, loc_flag_light_effect2, loc_flag_shading_effect;

// for skybox shaders
GLuint h_ShaderProgram_skybox;
GLint loc_cubemap_skybox;
GLint loc_ModelViewProjectionMatrix_SKY;

#define TEXTURE_INDEX_DIFFUSE	(0)
#define TEXTURE_INDEX_NORMAL	(1)
#define TEXTURE_INDEX_SPECULAR	(2)
#define TEXTURE_INDEX_EMISSIVE	(3)
#define TEXTURE_INDEX_SKYMAP	(4)

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2
#define LOC_TEMPCOORD 3

#define GLUT_SCROLL_UP 3
#define GLUT_SCROLL_DOWN 4

// for tiger animation
unsigned int timestamp_scene = 0; // the global clock in the scene
unsigned int timestamp_for_tiger = 0; // the clock for the tiger animation
int flag_animation, flag_polygon_fill, flag_blending = 0;
int cur_frame_tiger = 0, cur_frame_ben = 0, cur_frame_wolf, cur_frame_spider = 0;
float rotation_angle_tiger = 0.0f;

int flag_light_effect = 0, flag_light_effect2 = 0, flag_shading_effect = 0;

// for interactive viewing
int window_width, window_height;
int prev_x, prev_y;
int space_pressed, z_pressed, tiger_view_active, tiger_rear_view_active;
int left_mouse_pressed, right_mouse_pressed, middle_mouse_pressed;
float z_distance = 1000.0f, z_x, z_y;

int use_gs = 0; // whether to use Gouraud shading or not
float cube_alpha = 0.5;

#define MAX_Z_DISTANCE 10000.0f
#define MIN_Z_DISTANCE 300.0f

/* User Defined Function */
void update_camera_orientation_20171665(int axis, float rotation_angle);

int read_geometry_vnt(GLfloat** object, int bytes_per_primitive, char* filename) {
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
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

// include glm/*.hpp only if necessary
// #include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> //inverseTranspose, etc.
glm::mat4 ViewProjectionMatrix, ViewMatrix, ProjectionMatrix;
glm::mat4 ModelViewProjectionMatrix; // This one is sent to vertex shader when ready.
glm::mat4 ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;

glm::mat4 GlobalTigerModelMatrix;
glm::vec3 sphere_position;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define PI 3.14159265358979323846f

/******************************** START: objects ********************************/
// texture stuffs
#define TEXTURE_FLOOR				(0)
#define TEXTURE_TIGER				(1)
#define TEXTURE_WOLF				(2)
#define TEXTURE_SPIDER				(3)
#define TEXTURE_DRAGON				(4)
#define TEXTURE_OPTIMUS				(5)
#define TEXTURE_COW					(6)
#define TEXTURE_BUS					(7)
#define TEXTURE_BIKE				(8)
#define TEXTURE_GODZILLA			(9)
#define TEXTURE_IRONMAN				(10)
#define TEXTURE_TANK				(11)
#define TEXTURE_NATHAN				(12)
#define TEXTURE_OGRE				(13)
#define TEXTURE_CAT					(14)
#define TEXTURE_ANT					(15)
#define TEXTURE_TOWER				(16)
#define TEXTURE_MY_CUBE				(17)
#define N_TEXTURES_USED				(18)

GLuint texture_names[N_TEXTURES_USED];

// texture id
#define TEXTURE_ID_DIFFUSE	(0)
#define TEXTURE_ID_NORMAL	(1)

void My_glTexImage2D_from_file(const char* filename) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap, * tx_pixmap_32;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	FreeImage_FlipVertical(tx_pixmap);

	fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);
	if (tx_bits_per_pixel == 32)
		tx_pixmap_32 = tx_pixmap;
	else {
		fprintf(stdout, " * Converting texture from %d bits to 32 bits...\n", tx_bits_per_pixel);
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
	}

	width = FreeImage_GetWidth(tx_pixmap_32);
	height = FreeImage_GetHeight(tx_pixmap_32);
	data = FreeImage_GetBits(tx_pixmap_32);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap_32);
	if (tx_bits_per_pixel != 32)
		FreeImage_Unload(tx_pixmap);
}

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
	CAMERA_ANIMATION,
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
void set_ViewMatrix_from_camera_frame(void) {
	ViewMatrix = glm::mat4(current_camera.uaxis[0], current_camera.vaxis[0], current_camera.naxis[0], 0.0f,
		current_camera.uaxis[1], current_camera.vaxis[1], current_camera.naxis[1], 0.0f,
		current_camera.uaxis[2], current_camera.vaxis[2], current_camera.naxis[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-current_camera.pos[0], -current_camera.pos[1], -current_camera.pos[2]));
}

void set_current_camera(int camera_num) {
	Camera* pCamera = &camera_info[camera_num];

	memcpy(&current_camera, pCamera, sizeof(Camera));
	current_camera.aspect_ratio = (float)window_width / (float)window_height;
	set_ViewMatrix_from_camera_frame();

	if (camera_num == CAMERA_TIGER || camera_num == CAMERA_TIGER_REAR) {
		if (flag_animation) {
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
		set_ViewMatrix_from_camera_frame();
		ViewMatrix = ViewMatrix * glm::inverse(GlobalTigerModelMatrix);
	}
	else {
		set_ViewMatrix_from_camera_frame();
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

	//CAMERA_ANIMATION : animation mode view
	pCamera = &camera_info[CAMERA_ANIMATION];
	pCamera->pos[0] = 139.323364f; pCamera->pos[1] = 1070.164734f; pCamera->pos[2] = 473.017883f;
	pCamera->uaxis[0] = 1.0f; pCamera->uaxis[1] = 0.0f; pCamera->uaxis[2] = 0.0f;
	pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 0.0f; pCamera->vaxis[2] = 1.0f;
	pCamera->naxis[0] = 0.0f; pCamera->naxis[1] = -1.0f; pCamera->naxis[2] = 0.0f;
	pCamera->move = 1;
	pCamera->fovy = 1.099558, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_TIGER: tiger view 
	pCamera = &camera_info[CAMERA_TIGER];
	pCamera->pos[0] = 0.0f; pCamera->pos[1] = -100.0f; pCamera->pos[2] = 62.0f;
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


void rotate_camera_to_point(int x, int y) {
	glm::mat3 ViewMatrixInverse = glm::mat3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2],
		current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2],
		current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]
	);

	glm::vec3 viewDirection = ViewMatrixInverse * glm::vec3(0.0f, 0.0f, -1.0f);
	float near_clip_height = 2.0f * current_camera.near_c * glm::tan(current_camera.fovy / 2.0f);
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

	set_ViewMatrix_from_camera_frame();
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

	ShaderInfo shader_info_TXPS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");

	for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].shadow_on", i);
		loc_light[i].shadow_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position_MC", i);
		loc_light[i].position_MC = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");

	loc_material.diffuseTex = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");
	loc_material.normalTex = glGetUniformLocation(h_ShaderProgram_TXPS, "u_normal_texture");
	loc_material.emissiveTex = glGetUniformLocation(h_ShaderProgram_TXPS, "u_emissive_texture");

	loc_flag_diffuse_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_diffuse_texture_mapping");
	loc_flag_normal_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_normal_texture_mapping");
	loc_flag_emissive_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_emissive_texture_mapping");
	loc_flag_normal_based_directX = glGetUniformLocation(h_ShaderProgram_TXPS, "u_normal_based_directX");
	loc_flag_blending = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_blending");

	loc_timestamp = glGetUniformLocation(h_ShaderProgram_TXPS, "u_timestamp");

	loc_flag_light_effect = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_light_effect");
	loc_flag_light_effect2 = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_light_effect2");
	loc_flag_shading_effect = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_shading_effect");

	loc_flag_fog = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_fog");

	loc_fragment_alpha = glGetUniformLocation(h_ShaderProgram_TXPS, "u_fragment_alpha");

	ShaderInfo shader_info_skybox[3] = {
	{ GL_VERTEX_SHADER, "Shaders/skybox.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/skybox.frag" },
	{ GL_NONE, NULL }
	};

	h_ShaderProgram_skybox = LoadShaders(shader_info_skybox);
	loc_cubemap_skybox = glGetUniformLocation(h_ShaderProgram_skybox, "u_skymap");
	loc_ModelViewProjectionMatrix_SKY = glGetUniformLocation(h_ShaderProgram_skybox, "u_ModelViewProjectionMatrix");

	ShaderInfo shader_info_GS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Gouraud.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Gouraud.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_GS = LoadShaders(shader_info_GS);
	glUseProgram(h_ShaderProgram_GS);

	loc_ModelViewProjectionMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_global_ambient_color");

	for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light_GS[i].light_on = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].shadow_on", i);
		loc_light_GS[i].shadow_on = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light_GS[i].position = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light_GS[i].ambient_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light_GS[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light_GS[i].specular_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light_GS[i].spot_direction = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light_GS[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light_GS[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light_GS[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_GS, string);
	}


	loc_material_GS.ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.ambient_color");
	loc_material_GS.diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.diffuse_color");
	loc_material_GS.specular_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_color");
	loc_material_GS.emissive_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.emissive_color");
	loc_material_GS.specular_exponent = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_exponent");
}


void set_material(Material_Parameters* material_parameters) {
	// assume ShaderProgram_TXPS is used
	glUniform4fv(loc_material.ambient_color, 1, material_parameters->ambient_color);
	glUniform4fv(loc_material.diffuse_color, 1, material_parameters->diffuse_color);
	glUniform4fv(loc_material.specular_color, 1, material_parameters->specular_color);
	glUniform1f(loc_material.specular_exponent, material_parameters->specular_exponent);
	glUniform4fv(loc_material.emissive_color, 1, material_parameters->emissive_color);
}

void set_material_GS(Material_Parameters* material_parameters) {
	// assume ShaderProgram_GS is used
	glUniform4fv(loc_material_GS.ambient_color, 1, material_parameters->ambient_color);
	glUniform4fv(loc_material_GS.diffuse_color, 1, material_parameters->diffuse_color);
	glUniform4fv(loc_material_GS.specular_color, 1, material_parameters->specular_color);
	glUniform1f(loc_material_GS.specular_exponent, material_parameters->specular_exponent);
	glUniform4fv(loc_material_GS.emissive_color, 1, material_parameters->emissive_color);
}


void bind_texture(int glTextureId, GLuint texture_name) {
	glActiveTexture(GL_TEXTURE0 + glTextureId);
	glBindTexture(GL_TEXTURE_2D, texture_name);
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

//bistro_exterior
GLuint* bistro_exterior_VBO;
GLuint* bistro_exterior_VAO;
int* bistro_exterior_n_triangles;
int* bistro_exterior_vertex_offset;
GLfloat** bistro_exterior_vertices;
GLuint* bistro_exterior_texture_names;

bool* flag_texture_mapping;

bool readTexImage2D_from_file(const char* filename) {
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

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	// VBO, VAO mallocflag_texture_mapping
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
	glUseProgram(h_ShaderProgram_TXPS);

	ModelViewMatrix = ViewMatrix;
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	for (int materialIdx = 0; materialIdx < scene.n_materials; materialIdx++) {
		// set material
		glUniform4fv(loc_material.ambient_color, 1, scene.material_list[materialIdx].shading.ph.ka);
		glUniform4fv(loc_material.diffuse_color, 1, scene.material_list[materialIdx].shading.ph.kd);
		glUniform4fv(loc_material.specular_color, 1, scene.material_list[materialIdx].shading.ph.ks);
		glUniform1f(loc_material.specular_exponent, scene.material_list[materialIdx].shading.ph.spec_exp);
		glUniform4fv(loc_material.emissive_color, 1, scene.material_list[materialIdx].shading.ph.kr);

		int diffuseTexId = scene.material_list[materialIdx].diffuseTexId;
		int normalTexId = scene.material_list[materialIdx].normalMapTexId;
		int emissiveTexId = scene.material_list[materialIdx].emissiveTexId;

		bindTexture(loc_material.diffuseTex, TEXTURE_INDEX_DIFFUSE, diffuseTexId);
		glUniform1i(loc_flag_diffuse_texture_mapping, flag_texture_mapping[diffuseTexId]);
		bindTexture(loc_material.normalTex, TEXTURE_INDEX_NORMAL, normalTexId);
		glUniform1i(loc_flag_normal_texture_mapping, flag_texture_mapping[normalTexId]);
		glUniform1i(loc_flag_normal_based_directX, 1); // only for bistro exterior
		bindTexture(loc_material.emissiveTex, TEXTURE_INDEX_EMISSIVE, emissiveTexId);
		glUniform1i(loc_flag_emissive_texture_mapping, flag_texture_mapping[emissiveTexId]);

		glEnable(GL_TEXTURE_2D);

		glBindVertexArray(bistro_exterior_VAO[materialIdx]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * bistro_exterior_n_triangles[materialIdx]);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUniform1i(loc_flag_normal_texture_mapping ,false);
	glUniform1i(loc_flag_normal_based_directX, 0); // only for bistro exterior
	glUniform1i(loc_flag_emissive_texture_mapping, false);
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
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(20000.0f, 20000.0f, 20000.0f));
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

Material_Parameters material_tiger;

void prepare_tiger_20171665(void) { // vertices enumerated clockwise
	int i, n_bytes_per_vertex, n_bytes_per_triangle, tiger_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/tiger/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		tiger_n_triangles[i] = read_geometry_vnt(&tiger_vertices[i], n_bytes_per_triangle, filename);
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

	material_tiger.ambient_color[0] = 0.24725f;
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

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_TIGER]);

	readTexImage2D_from_file("snow.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
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
	glUseProgram(h_ShaderProgram_TXPS);
	set_material(&material_tiger);
	bind_texture(TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_TIGER]);

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

	float start_x3 = start_x2 + -(distance2)*glm::cos(TIGER_DEGREE2 * TO_RADIAN);
	float start_y3 = start_y2 + distance2 * glm::sin(TIGER_DEGREE2 * TO_RADIAN);

	float center_x1 = start_x3 + radius3 * glm::sin(TIGER_DEGREE2 * TO_RADIAN);
	float center_y1 = start_y3 + radius3 * glm::cos(TIGER_DEGREE2 * TO_RADIAN);

	float center_x2 = start_x2 + -(distance2 + sqrt(3.0f) * radius3) * glm::cos(TIGER_DEGREE2 * TO_RADIAN);
	float center_y2 = start_y2 + (distance2 + sqrt(3.0f) * radius3) * glm::sin(TIGER_DEGREE2 * TO_RADIAN);

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
			(180.0f - TIGER_DEGREE2 + 30.0f + time_factor) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f)
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
	GlobalTigerModelMatrix = TigerModelMatrix;
	ModelViewMatrix = ViewMatrix * TigerModelMatrix;
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	glUniform1i(loc_flag_diffuse_texture_mapping, true);
	glUniform1i(loc_flag_normal_texture_mapping, false);
	glUniform1i(loc_flag_emissive_texture_mapping, false);

	glBindVertexArray(tiger_VAO);
	glDrawArrays(GL_TRIANGLES, tiger_vertex_offset[cur_frame_tiger], 3 * tiger_n_triangles[cur_frame_tiger]);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}



// spider object
#define N_SPIDER_FRAMES 16
GLuint spider_VBO, spider_VAO;
int spider_n_triangles[N_SPIDER_FRAMES];
int spider_vertex_offset[N_SPIDER_FRAMES];
GLfloat* spider_vertices[N_SPIDER_FRAMES];

Material_Parameters material_spider;

void prepare_spider_20171665(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, spider_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_SPIDER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/spider/spider_vnt_%d%d.geom", i / 10, i % 10);
		spider_n_triangles[i] = read_geometry_vnt(&spider_vertices[i], n_bytes_per_triangle, filename);
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

	material_spider.ambient_color[0] = 0.5f;
	material_spider.ambient_color[1] = 0.5f;
	material_spider.ambient_color[2] = 0.5f;
	material_spider.ambient_color[3] = 1.0f;

	material_spider.diffuse_color[0] = 0.9f;
	material_spider.diffuse_color[1] = 0.5f;
	material_spider.diffuse_color[2] = 0.1f;
	material_spider.diffuse_color[3] = 1.0f;

	material_spider.specular_color[0] = 0.5f;
	material_spider.specular_color[1] = 0.5f;
	material_spider.specular_color[2] = 0.5f;
	material_spider.specular_color[3] = 1.0f;

	material_spider.specular_exponent = 11.334717f;

	material_spider.emissive_color[0] = 0.000000f;
	material_spider.emissive_color[1] = 0.000000f;
	material_spider.emissive_color[2] = 0.000000f;
	material_spider.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_SPIDER]);

	My_glTexImage2D_from_file("snow.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}


void draw_spider_20171665(void) {
	glUseProgram(h_ShaderProgram_TXPS);
	set_material(&material_spider);
	bind_texture(TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_SPIDER]);
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

	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	glUniform1i(loc_flag_diffuse_texture_mapping, true);
	glUniform1i(loc_flag_normal_texture_mapping, false);
	glUniform1i(loc_flag_emissive_texture_mapping, false);

	glBindVertexArray(spider_VAO);
	glDrawArrays(GL_TRIANGLES, spider_vertex_offset[cur_frame_spider], 3 * spider_n_triangles[cur_frame_spider]);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}



// wolf object
#define N_WOLF_FRAMES 17
GLuint wolf_VBO, wolf_VAO;
int wolf_n_triangles[N_WOLF_FRAMES];
int wolf_vertex_offset[N_WOLF_FRAMES];
GLfloat* wolf_vertices[N_WOLF_FRAMES];

Material_Parameters material_wolf;

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
		wolf_n_triangles[i] = read_geometry_vnt(&wolf_vertices[i], n_bytes_per_triangle, filename);
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

	material_wolf.ambient_color[0] = 0.24725f;
	material_wolf.ambient_color[1] = 0.1995f;
	material_wolf.ambient_color[2] = 0.1745f;
	material_wolf.ambient_color[3] = 1.0f;

	material_wolf.diffuse_color[0] = 0.45164f;
	material_wolf.diffuse_color[1] = 0.30648f;
	material_wolf.diffuse_color[2] = 0.22648f;
	material_wolf.diffuse_color[3] = 1.0f;

	material_wolf.specular_color[0] = 0.2f;
	material_wolf.specular_color[1] = 0.3f;
	material_wolf.specular_color[2] = 0.2f;
	material_wolf.specular_color[3] = 1.0f;

	material_wolf.specular_exponent = 1.2f;

	material_wolf.emissive_color[0] = 0.1f;
	material_wolf.emissive_color[1] = 0.1f;
	material_wolf.emissive_color[2] = 0.0f;
	material_wolf.emissive_color[3] = 1.0f;

	/*
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_WOLF]);

	My_glTexImage2D_from_file("Data/dynamic_objects/wolf/Wolf_Body.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);*/
}

void draw_wolf_20171665(void) {
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

	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	if (!use_gs) {
		glUseProgram(h_ShaderProgram_TXPS);
		set_material(&material_wolf);
		glFrontFace(GL_CW);

		//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

		glUniform1i(loc_flag_diffuse_texture_mapping, false);
		glUniform1i(loc_flag_normal_texture_mapping, false);
		glUniform1i(loc_flag_emissive_texture_mapping, false);

		glBindVertexArray(wolf_VAO);
		glDrawArrays(GL_TRIANGLES, wolf_vertex_offset[cur_frame_wolf], 3 * wolf_n_triangles[cur_frame_wolf]);
		glBindVertexArray(0);

		glUniform1i(loc_flag_diffuse_texture_mapping, true);

		glUseProgram(0);
	}
	else {
		glUseProgram(h_ShaderProgram_GS);
        set_material_GS(&material_wolf);
        glFrontFace(GL_CW);

        glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
        glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
        glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

        glBindVertexArray(wolf_VAO);
        glDrawArrays(GL_TRIANGLES, wolf_vertex_offset[cur_frame_wolf], 3 * wolf_n_triangles[cur_frame_wolf]);
        glBindVertexArray(0);

        glUseProgram(0);
	}
}



// bus object
GLuint bus_VBO, bus_VAO;
int bus_n_triangles;
GLfloat* bus_vertices;

Material_Parameters material_bus;

void prepare_bus_20171665(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, bus_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/bus_vnt.geom");
	bus_n_triangles = read_geometry_vnt(&bus_vertices, n_bytes_per_triangle, filename);
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

	material_bus.ambient_color[0] = 0.4f;
	material_bus.ambient_color[1] = 0.4f;
	material_bus.ambient_color[2] = 0.4f;
	material_bus.ambient_color[3] = 1.0f;

	material_bus.diffuse_color[0] = 0.96862f;
	material_bus.diffuse_color[1] = 0.90980f;
	material_bus.diffuse_color[2] = 0.79607f;
	material_bus.diffuse_color[3] = 1.0f;

	material_bus.specular_color[0] = 0.5f;
	material_bus.specular_color[1] = 0.5f;
	material_bus.specular_color[2] = 0.5f;
	material_bus.specular_color[3] = 1.0f;

	material_bus.specular_exponent = 50.334717f;

	material_bus.emissive_color[0] = 0.000000f;
	material_bus.emissive_color[1] = 0.000000f;
	material_bus.emissive_color[2] = 0.000000f;
	material_bus.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_BUS]);

	My_glTexImage2D_from_file("Data/static_objects/Bus01_A.tga");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}


void draw_bus_20171665(void) {
	glUseProgram(h_ShaderProgram_TXPS);
	set_material(&material_bus);
	bind_texture(TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_BUS]);

	glFrontFace(GL_CW);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(4798.0f, -1322.0f, 20.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 125.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	glUniform1i(loc_flag_diffuse_texture_mapping, true);
	glUniform1i(loc_flag_normal_texture_mapping, false);
	glUniform1i(loc_flag_emissive_texture_mapping, false);


	glBindVertexArray(bus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * bus_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}



// optimus object
GLuint optimus_VBO, optimus_VAO;
int optimus_n_triangles;
GLfloat* optimus_vertices;

Material_Parameters material_optimus;

void prepare_optimus_20171665(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, optimus_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/optimus_vnt.geom");
	optimus_n_triangles = read_geometry_vnt(&optimus_vertices, n_bytes_per_triangle, filename);
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


	material_optimus.ambient_color[0] = 0.5f;
	material_optimus.ambient_color[1] = 0.5f;
	material_optimus.ambient_color[2] = 0.5f;
	material_optimus.ambient_color[3] = 1.0f;

	material_optimus.diffuse_color[0] = 0.2f;
	material_optimus.diffuse_color[1] = 0.2f;
	material_optimus.diffuse_color[2] = 0.9f;
	material_optimus.diffuse_color[3] = 1.0f;

	material_optimus.specular_color[0] = 1.0f;
	material_optimus.specular_color[1] = 1.0f;
	material_optimus.specular_color[2] = 1.0f;
	material_optimus.specular_color[3] = 1.0f;

	material_optimus.specular_exponent = 52.334717f;

	material_optimus.emissive_color[0] = 0.000000f;
	material_optimus.emissive_color[1] = 0.000000f;
	material_optimus.emissive_color[2] = 0.000000f;
	material_optimus.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_OPTIMUS]);

	My_glTexImage2D_from_file("sunset.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_optimus_20171665(void) {
	glUseProgram(h_ShaderProgram_TXPS);
	set_material(&material_optimus);
	bind_texture(TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_OPTIMUS]);

	glFrontFace(GL_CW);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-1750.0f, 905.0f, 10.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -30.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	glUniform1i(loc_flag_diffuse_texture_mapping, true);
	glUniform1i(loc_flag_normal_texture_mapping, false);
	glUniform1i(loc_flag_emissive_texture_mapping, false);

	glBindVertexArray(optimus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * optimus_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(sphere_VAO);
	glUniform3fv(loc_primitive_color, 1, grid_color);
	glDrawArrays(GL_TRIANGLES, 0, num_sphere_vertices);
	glBindVertexArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(0);
}


// my_cube object

GLuint my_cube_VBO, my_cube_VAO;

const int num_my_cube_triangles = 12;
const int num_my_cube_vertices = num_my_cube_triangles * 3;

Material_Parameters material_my_cube;

GLfloat my_cube_vertices[num_my_cube_vertices][10] = {
	// Triangle 1
	{-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f},

	// Triangle 2
	{1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
	{-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f},

	// Triangle 3
	{-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	{1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},

	// Triangle 4
	{1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},

	// Triangle 5
	{-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	{1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f ,0.0f, 1.0f ,0.0f},
	{1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},

	// Triangle 6
	{1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},

	// Triangle 7
	{1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	{1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},

	// Triangle 8
	{1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},

	// Triangle 9
	{-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	{-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
	{-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},

	// Triangle 10
	{-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},

	// Triangle 11
	{1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},

	// Triangle 12
	{-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	{1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
	{-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f}
};

void prepare_cube_20171665(void) {
	int n_bytes_per_vertex;

	n_bytes_per_vertex = 10 * sizeof(float);

	glGenBuffers(1, &my_cube_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, my_cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, num_my_cube_vertices * n_bytes_per_vertex, &my_cube_vertices[0][0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &my_cube_VAO);
	glBindVertexArray(my_cube_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, my_cube_VAO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(LOC_TEMPCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_my_cube.ambient_color[0] = 0.5f;
	material_my_cube.ambient_color[1] = 0.5f;
	material_my_cube.ambient_color[2] = 0.5f;
	material_my_cube.ambient_color[3] = 1.0f;

	material_my_cube.diffuse_color[0] = 0.7f;
	material_my_cube.diffuse_color[1] = 0.5f;
	material_my_cube.diffuse_color[2] = 0.2f;
	material_my_cube.diffuse_color[3] = 1.0f;

	material_my_cube.specular_color[0] = 0.4f;
	material_my_cube.specular_color[1] = 0.4f;
	material_my_cube.specular_color[2] = 0.2f;
	material_my_cube.specular_color[3] = 1.0f;

	material_my_cube.specular_exponent = 5.334717f;

	material_my_cube.emissive_color[0] = 0.000000f;
	material_my_cube.emissive_color[1] = 0.000000f;
	material_my_cube.emissive_color[2] = 0.000000f;
	material_my_cube.emissive_color[3] = 1.0f;


	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_MY_CUBE]);

	My_glTexImage2D_from_file("Data/static_objects/checker_tex.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

}

void draw_cube_20171665(void) {
	glUseProgram(h_ShaderProgram_TXPS);

	if (flag_blending) {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(loc_flag_blending, true);
		glEnable(GL_CULL_FACE);
	}
	else {
		glUniform1i(loc_flag_blending, false);
	}

	glFrontFace(GL_CCW);

	set_material(&material_my_cube);
	bind_texture(TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_MY_CUBE]);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUniform1f(loc_fragment_alpha, cube_alpha);
	glUniform1i(loc_flag_shading_effect, flag_shading_effect);
	

	float time_factor = timestamp_scene % 360;

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-815.f, 300.f, 1000.f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.f, 100.f, 100.f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, time_factor * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	glUniform1i(loc_flag_diffuse_texture_mapping, true);
	glUniform1i(loc_flag_normal_texture_mapping, false);
	glUniform1i(loc_flag_emissive_texture_mapping, false);

	glBindVertexArray(my_cube_VAO);

	if (flag_blending) {
		glCullFace(GL_FRONT);
		glDrawArrays(GL_TRIANGLES, 0, num_my_cube_vertices);
		glCullFace(GL_BACK);
		glDrawArrays(GL_TRIANGLES, 0, num_my_cube_vertices);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, num_my_cube_vertices);
	}
	glDrawArrays(GL_TRIANGLES, 0, num_my_cube_vertices);
	glBindVertexArray(0);

	if (flag_blending) {
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glUniform1i(loc_flag_blending, false);
	}

	glUniform1i(loc_flag_shading_effect, false);

	glUseProgram(0);
}


/*****************************  END: geometry setup *****************************/

/********************************  START: light *********************************/
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

#define LIGHT_SQUARE				(0)
#define LIGHT_CAMERA                (1)
#define LIGHT_TIGER 				(2)
#define LIGHT_ROUNDABOUT 			(3)
#define LIGHT_NUMBER				(4)

void initialize_lights(void) { // follow OpenGL conventions for initialization
	//printf("scene.n_lights: %d\n", scene.n_lights);

	if (scene.n_lights + LIGHT_NUMBER > NUMBER_OF_LIGHT_SUPPORTED) {
		printf("Number of lights exceeds maximum. \n");
		return;
	}

	for (int i = 0; i < scene.n_lights; i++) {
		light[i].light_on = 1;

		if (LIGHT_DIRECTIONAL == scene.light_list[i].type) {
			light[i].position[0] = scene.light_list[i].pos[0];
			light[i].position[1] = scene.light_list[i].pos[1];
			light[i].position[2] = scene.light_list[i].pos[2];
			light[i].position[3] = 0.0f;

			light[i].ambient_color[0] = 0.0f;
			light[i].ambient_color[1] = 0.0f;
			light[i].ambient_color[2] = 0.0f;
			light[i].ambient_color[3] = 1.0f;

			light[i].diffuse_color[0] = scene.light_list[i].color[0];
			light[i].diffuse_color[1] = scene.light_list[i].color[1];
			light[i].diffuse_color[2] = scene.light_list[i].color[2];
			light[i].diffuse_color[3] = 1.0f;

			light[i].specular_color[0] = 0.5f;
			light[i].specular_color[1] = 0.5f;
			light[i].specular_color[2] = 0.5f;
			light[i].specular_color[3] = 1.0f;
		}
		else if (LIGHT_POINT == scene.light_list[i].type) {
			light[i].position[0] = scene.light_list[i].pos[0];
			light[i].position[1] = scene.light_list[i].pos[1];
			light[i].position[2] = scene.light_list[i].pos[2];
			light[i].position[3] = scene.light_list[i].pos[3];

			light[i].ambient_color[0] = 0.0f;
			light[i].ambient_color[1] = 0.0f;
			light[i].ambient_color[2] = 0.0f;
			light[i].ambient_color[3] = 1.0f;

			light[i].diffuse_color[0] = scene.light_list[i].color[0];
			light[i].diffuse_color[1] = scene.light_list[i].color[1];
			light[i].diffuse_color[2] = scene.light_list[i].color[2];
			light[i].diffuse_color[3] = 1.0f;

			light[i].specular_color[0] = 0.8f;
			light[i].specular_color[1] = 0.8f;
			light[i].specular_color[2] = 0.8f;
			light[i].specular_color[3] = 1.0f;

			light[i].light_attenuation_factors[0] = 1.0f;
			light[i].light_attenuation_factors[1] = 0.01;
			light[i].light_attenuation_factors[2] = 0.0f;
			light[i].light_attenuation_factors[3] = 1.0f;

			light[i].spot_cutoff_angle = 180.0f;
		}
		else {
			// for spot light, volume light, ...
		}
	}

	// User-defined Light
	// Light 1
	int user_light_1 = scene.n_lights + LIGHT_SQUARE;
	light[user_light_1].light_on = 1; // turn off the light initially
	light[user_light_1].position[0] = -955.0f;
	light[user_light_1].position[1] = 223.0f;
	light[user_light_1].position[2] = 2200.0f;
	light[user_light_1].position[3] = 1.0f;

	light[user_light_1].ambient_color[0] = 0.5f;
	light[user_light_1].ambient_color[1] = 0.5f;
	light[user_light_1].ambient_color[2] = 0.5f;
	light[user_light_1].ambient_color[3] = 1.0f;

	light[user_light_1].diffuse_color[0] = 0.8f;
	light[user_light_1].diffuse_color[1] = 0.8f;
	light[user_light_1].diffuse_color[2] = 0.0f;
	light[user_light_1].diffuse_color[3] = 1.0f;

	light[user_light_1].specular_color[0] = 0.0f;
	light[user_light_1].specular_color[1] = 0.8f;
	light[user_light_1].specular_color[2] = 0.8f;
	light[user_light_1].specular_color[3] = 1.0f;

	light[user_light_1].spot_direction[0] = 0.0f;
	light[user_light_1].spot_direction[1] = 0.0f;
	light[user_light_1].spot_direction[2] = -1.0f;

	light[user_light_1].spot_exponent = 1.0f;

	light[user_light_1].spot_cutoff_angle = 70.0f;

	light[user_light_1].light_attenuation_factors[0] = 1.0f;
	light[user_light_1].light_attenuation_factors[1] = 0.001f;
	light[user_light_1].light_attenuation_factors[2] = 0.0f;
	light[user_light_1].light_attenuation_factors[3] = 1.0f;


	// Light 2
	int user_light_2 = scene.n_lights + LIGHT_CAMERA;
	light[user_light_2].light_on = 1; // turn off the light initially
	light[user_light_2].position[0] = 0.0f;
	light[user_light_2].position[1] = -90.0f;
	light[user_light_2].position[2] = 100.0f;
	light[user_light_2].position[3] = 1.0f;

	light[user_light_2].ambient_color[0] = 0.2f;
	light[user_light_2].ambient_color[1] = 0.1f;
	light[user_light_2].ambient_color[2] = 0.6f;

	light[user_light_2].diffuse_color[0] = 0.2f;
	light[user_light_2].diffuse_color[1] = 0.4f;
	light[user_light_2].diffuse_color[2] = 0.8f;
	light[user_light_2].diffuse_color[3] = 1.0f;

	light[user_light_2].specular_color[0] = 0.5f;
	light[user_light_2].specular_color[1] = 0.5f;
	light[user_light_2].specular_color[2] = 0.9f;
	light[user_light_2].specular_color[3] = 1.0f;

	light[user_light_2].spot_direction[0] = 0.0f;
	light[user_light_2].spot_direction[1] = 0.0f;
	light[user_light_2].spot_direction[2] = -1.0f;

	light[user_light_2].spot_exponent = 1.1f;

	light[user_light_2].spot_cutoff_angle = 15.0f;

	light[user_light_2].light_attenuation_factors[0] = 1.0f;
	light[user_light_2].light_attenuation_factors[1] = 0.0005f;
	light[user_light_2].light_attenuation_factors[2] = 0.0f;
	light[user_light_2].light_attenuation_factors[3] = 1.0f;


	// Light 3
	int user_light_3 = scene.n_lights + LIGHT_TIGER;
	light[user_light_3].light_on = 1; // turn off the light initially
	light[user_light_3].position[0] = 0.0f;
	light[user_light_3].position[1] = -90.0f;
	light[user_light_3].position[2] = 100.0f;
	light[user_light_3].position[3] = 1.0f;

	light[user_light_3].ambient_color[0] = 0.2f;
	light[user_light_3].ambient_color[1] = 0.5f;
	light[user_light_3].ambient_color[2] = 0.2f;

	light[user_light_3].diffuse_color[0] = 0.2f;
	light[user_light_3].diffuse_color[1] = 0.8f;
	light[user_light_3].diffuse_color[2] = 0.4f;
	light[user_light_3].diffuse_color[3] = 1.0f;

	light[user_light_3].specular_color[0] = 0.6f;
	light[user_light_3].specular_color[1] = 0.9f;
	light[user_light_3].specular_color[2] = 0.6f;
	light[user_light_3].specular_color[3] = 1.0f;

	light[user_light_3].spot_direction[0] = 0.0f;
	light[user_light_3].spot_direction[1] = -1.0f;
	light[user_light_3].spot_direction[2] = 0.0f;

	light[user_light_3].spot_exponent = 1.2f;

	light[user_light_3].spot_cutoff_angle = 12.0f;

	light[user_light_3].light_attenuation_factors[0] = 1.0f;
	light[user_light_3].light_attenuation_factors[1] = 0.002f;
	light[user_light_3].light_attenuation_factors[2] = 0.0f;
	light[user_light_3].light_attenuation_factors[3] = 1.0f;


	// Light 3
	int user_light_4 = scene.n_lights + LIGHT_ROUNDABOUT;
	light[user_light_4].light_on = 1; // turn off the light initially
	light[user_light_4].position[0] = 4346.0f;
	light[user_light_4].position[1] = -2392.0f;
	light[user_light_4].position[2] = 1500.0f;
	light[user_light_4].position[3] = 1.0f;

	light[user_light_4].ambient_color[0] = 0.35f;
	light[user_light_4].ambient_color[1] = 0.5f;
	light[user_light_4].ambient_color[2] = 0.2f;

	light[user_light_4].diffuse_color[0] = 0.9f;
	light[user_light_4].diffuse_color[1] = 0.2f;
	light[user_light_4].diffuse_color[2] = 0.4f;
	light[user_light_4].diffuse_color[3] = 1.0f;

	light[user_light_4].specular_color[0] = 0.8f;
	light[user_light_4].specular_color[1] = 0.5f;
	light[user_light_4].specular_color[2] = 0.6f;
	light[user_light_4].specular_color[3] = 1.0f;

	light[user_light_4].spot_direction[0] = 0.0f;
	light[user_light_4].spot_direction[1] = 0.0f;
	light[user_light_4].spot_direction[2] = -1.0f;

	light[user_light_4].spot_exponent = 1.2f;

	light[user_light_4].spot_cutoff_angle = 60.0f;

	light[user_light_4].light_attenuation_factors[0] = 1.0f;
	light[user_light_4].light_attenuation_factors[1] = 0.0f;
	light[user_light_4].light_attenuation_factors[2] = 0.0f;
	light[user_light_4].light_attenuation_factors[3] = 1.0f;

}

void set_lights(void) {
	glUseProgram(h_ShaderProgram_TXPS);

	glUniform4f(loc_global_ambient_color, 0.7f, 0.7f, 0.7f, 1.0f);

	glm::vec4 light_position_EC;
	glm::vec4 light_position_MC;
	glm::vec3 light_direction_EC;
	for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, light[i].light_on);
		glUniform1i(loc_light[i].shadow_on, light[i].shadow_on);

		if (i == scene.n_lights + LIGHT_CAMERA) {
			if (tiger_view_active || tiger_rear_view_active){
				glUniform1i(loc_light[i].light_on, false);
				continue;
			}
			else {
				light_position_EC = ViewMatrix * glm::vec4(current_camera.pos[0], current_camera.pos[1], current_camera.pos[2], 1.0f);
			}
		}
		else if (i == scene.n_lights + LIGHT_TIGER) {
			light_position_EC = ViewMatrix * GlobalTigerModelMatrix * glm::vec4(light[i].position[0], light[i].position[1], light[i].position[2], light[i].position[3]);
		}
		else {
			light_position_EC = ViewMatrix * glm::vec4(light[i].position[0], light[i].position[1], light[i].position[2], light[i].position[3]);
		}
		glUniform4fv(loc_light[i].position, 1, &light_position_EC[0]);

		if (i == scene.n_lights + LIGHT_ROUNDABOUT) {
			light_position_MC = glm::vec4(light[i].position[0], light[i].position[1], light[i].position[2], light[i].position[3]);
			glUniform4fv(loc_light[i].position_MC, 1, &light_position_MC[0]);
		}

		glUniform4fv(loc_light[i].ambient_color, 1, light[i].ambient_color);
		glUniform4fv(loc_light[i].diffuse_color, 1, light[i].diffuse_color);
		glUniform4fv(loc_light[i].specular_color, 1, light[i].specular_color);

		if (0.0f != light[i].position[3]) {
			if (i == scene.n_lights + LIGHT_CAMERA) {
				light_direction_EC = glm::transpose(glm::inverse(glm::mat3(ViewMatrix))) * glm::vec3(-current_camera.naxis[0], -current_camera.naxis[1], -current_camera.naxis[2]);
				//printf("light_direction_EC: (%lf, %lf, %lf)\n", light_direction_EC[0], light_direction_EC[1], light_direction_EC[2]);
			}
			else if (i == scene.n_lights + LIGHT_TIGER) {
				light_direction_EC = glm::transpose(glm::inverse(glm::mat3(ViewMatrix))) * glm::inverseTranspose(glm::mat3(GlobalTigerModelMatrix)) \
					* glm::vec3(light[i].spot_direction[0], light[i].spot_direction[1], light[i].spot_direction[2]);
			}
			else {
				light_direction_EC = glm::transpose(glm::inverse(glm::mat3(ViewMatrix))) * glm::vec3(light[i].spot_direction[0], light[i].spot_direction[1], light[i].spot_direction[2]);
			}

			glUniform3fv(loc_light[i].spot_direction, 1, &light_direction_EC[0]);
			glUniform1f(loc_light[i].spot_exponent, light[i].spot_exponent);
			glUniform1f(loc_light[i].spot_cutoff_angle, light[i].spot_cutoff_angle);
			glUniform4fv(loc_light[i].light_attenuation_factors, 1, light[i].light_attenuation_factors);
		}
	}

	glUseProgram(0);

	if (use_gs) {
		glUseProgram(h_ShaderProgram_GS);

		glUniform4f(loc_global_ambient_color_GS, 0.7f, 0.7f, 0.7f, 1.0f);

		glm::vec4 light_position_EC;
		glm::vec3 light_direction_EC;
		for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
			glUniform1i(loc_light_GS[i].light_on, light[i].light_on);
			glUniform1i(loc_light_GS[i].shadow_on, light[i].shadow_on);

			if (i == scene.n_lights + LIGHT_CAMERA) {
				if (tiger_view_active || tiger_rear_view_active) {
					glUniform1i(loc_light_GS[i].light_on, false);
					continue;
				}
				else {
					light_position_EC = ViewMatrix * glm::vec4(current_camera.pos[0], current_camera.pos[1], current_camera.pos[2], 1.0f);
				}
			}
			else if (i == scene.n_lights + LIGHT_TIGER) {
				light_position_EC = ViewMatrix * GlobalTigerModelMatrix * glm::vec4(light[i].position[0], light[i].position[1], light[i].position[2], light[i].position[3]);
			}
			else {
				light_position_EC = ViewMatrix * glm::vec4(light[i].position[0], light[i].position[1], light[i].position[2], light[i].position[3]);
			}
			glUniform4fv(loc_light_GS[i].position, 1, &light_position_EC[0]);

			glUniform4fv(loc_light_GS[i].ambient_color, 1, light[i].ambient_color);
			glUniform4fv(loc_light_GS[i].diffuse_color, 1, light[i].diffuse_color);
			glUniform4fv(loc_light_GS[i].specular_color, 1, light[i].specular_color);

			if (0.0f != light[i].position[3]) {
				if (i == scene.n_lights + LIGHT_CAMERA) {
					light_direction_EC = glm::transpose(glm::inverse(glm::mat3(ViewMatrix))) * glm::vec3(-current_camera.naxis[0], -current_camera.naxis[1], -current_camera.naxis[2]);
					//printf("light_direction_EC: (%lf, %lf, %lf)\n", light_direction_EC[0], light_direction_EC[1], light_direction_EC[2]);
				}
				else if (i == scene.n_lights + LIGHT_TIGER) {
					light_direction_EC = glm::transpose(glm::inverse(glm::mat3(ViewMatrix))) * glm::inverseTranspose(glm::mat3(GlobalTigerModelMatrix)) \
						* glm::vec3(light[i].spot_direction[0], light[i].spot_direction[1], light[i].spot_direction[2]);
				}
				else {
					light_direction_EC = glm::transpose(glm::inverse(glm::mat3(ViewMatrix))) * glm::vec3(light[i].spot_direction[0], light[i].spot_direction[1], light[i].spot_direction[2]);
				}

				glUniform3fv(loc_light_GS[i].spot_direction, 1, &light_direction_EC[0]);
				glUniform1f(loc_light_GS[i].spot_exponent, light[i].spot_exponent);
				glUniform1f(loc_light_GS[i].spot_cutoff_angle, light[i].spot_cutoff_angle);
				glUniform4fv(loc_light_GS[i].light_attenuation_factors, 1, light[i].light_attenuation_factors);
			}
		}

		glUseProgram(0);
	}
}
/*********************************  END: light **********************************/

/********************  START: callback function definitions *********************/

void timer_scene(int value) {
	timestamp_scene = (timestamp_scene + 1) % UINT_MAX;
	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1ui(loc_timestamp, timestamp_scene);
	glUseProgram(0);
	timestamp_for_tiger = (timestamp_for_tiger + 1) % UINT_MAX;
	cur_frame_tiger = timestamp_for_tiger % N_TIGER_FRAMES;
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
	draw_tiger_20171665();
	draw_spider_20171665();
	draw_wolf_20171665();
	draw_cube_20171665();
	draw_bus_20171665();
	draw_optimus_20171665();

	if (z_pressed) {
		draw_sphere_20171665(z_x, z_y, z_distance);
	}

	if (tiger_view_active) {
		current_camera_num = CAMERA_TIGER;
		set_current_camera(current_camera_num);
		update_view_projection_matrix_20171665();
	}
	else if (tiger_rear_view_active) {
		current_camera_num = CAMERA_TIGER_REAR;
		set_current_camera(current_camera_num);
		update_view_projection_matrix_20171665();
	}

	set_lights();

	glutSwapBuffers();
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

		if (right_mouse_pressed) {
			update_camera_orientation_20171665(Z_AXIS, prev_x - x);
		}

		set_ViewMatrix_from_camera_frame();
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
			if ((z_distance + 20.0f) < MAX_Z_DISTANCE && !left_mouse_pressed) {
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
		if (z_pressed) {
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

		set_ViewMatrix_from_camera_frame();
		update_view_projection_matrix_20171665();
		glutPostRedisplay();
		//print_camera_information_20171665();
	}

}

void keyboard(unsigned char key, int x, int y) {
	if (key == '1') {
		use_gs = 1;
		glutPostRedisplay();
		return;
	}
	
	if (key == '2') {
		use_gs = 0;
		glutPostRedisplay();
		return;
	}

	if ((key >= '3') && (key < '3' + LIGHT_NUMBER - 1)) {
		int light_ID = (int)(key - '3') + scene.n_lights;

		glUseProgram(h_ShaderProgram_TXPS);
		light[light_ID].light_on = 1 - light[light_ID].light_on;
		glUniform1i(loc_light[light_ID].light_on, light[light_ID].light_on);
		glUseProgram(0);

		glutPostRedisplay();
		return;
	}

	if (key == '6') {
		flag_blending = 1 - flag_blending;

		glutPostRedisplay();
		return;
	}

	if (key == '7') {
		flag_light_effect = 1 - flag_light_effect;
		
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_light_effect, flag_light_effect);
		glUseProgram(0);

		glutPostRedisplay();
		return;
	}

	if (key == '8') {
		flag_shading_effect = 1 - flag_shading_effect;

		glutPostRedisplay();
		return;
	}

	if (key == '9') {
		flag_light_effect2 = 1 - flag_light_effect2;

		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_light_effect2, flag_light_effect2);
		glUseProgram(0);

		glutPostRedisplay();
		return;
	}

	switch (key) {
	case 'f':
		b_draw_grid = b_draw_grid ? false : true;
		glutPostRedisplay();
		break;
	case 'a':
		tiger_view_active = 0;
		tiger_rear_view_active = 0;
		current_camera_num = CAMERA_ANIMATION;
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
	case 'r':
		if (flag_blending) {
			cube_alpha += 0.05f;
			if (cube_alpha > 1.0f) {
				cube_alpha = 1.0f;
			}
		}
		break;
		glutPostRedisplay();
	case 'R':
		if (flag_blending) {
			cube_alpha -= 0.05f;
			if (cube_alpha < 0.0f) {
				cube_alpha = 0.0f;
			}
		}
		glutPostRedisplay();
		break;
	case 'e': // toggle the animation effect.
		flag_animation = 1 - flag_animation;
		if (flag_animation) {
			//glutTimerFunc(100, timer_scene, 0);
			fprintf(stdout, "^^^ Animation mode ON.\n");
		}
		else
			fprintf(stdout, "^^^ Animation mode OFF.\n");
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

	glDeleteVertexArrays(1, &skybox_VAO);
	glDeleteBuffers(1, &skybox_VBO);

	glDeleteVertexArrays(1, &tiger_VAO);
	glDeleteBuffers(1, &tiger_VBO);

	glDeleteVertexArrays(1, &spider_VAO);
	glDeleteBuffers(1, &spider_VBO);

	glDeleteVertexArrays(1, &sphere_VAO);
	glDeleteBuffers(1, &sphere_VBO);

	glDeleteVertexArrays(1, &my_cube_VAO);
	glDeleteBuffers(1, &my_cube_VBO);

	glDeleteTextures(scene.n_textures, bistro_exterior_texture_names);

	free(bistro_exterior_n_triangles);
	free(bistro_exterior_vertex_offset);

	free(bistro_exterior_VAO);
	free(bistro_exterior_VBO);

	free(bistro_exterior_texture_names);
	free(flag_texture_mapping);

	glDeleteTextures(N_TEXTURES_USED, texture_names);
}
/*********************  END: callback function definitions **********************/

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutReshapeFunc(reshape);
	glutMotionFunc(mousemove);
	glutPassiveMotionFunc(mousepassivemove);
	glutMouseFunc(mousepress);
	glutSpecialFunc(special);
	glutCloseFunc(cleanup);
	glutTimerFunc(100, timer_scene, 0);
}

void initialize_flags(void) {
	flag_fog = 0;
	flag_animation = 1;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_flag_fog, flag_fog);
	glUseProgram(0);
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix = glm::mat4(1.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	initialize_lights();
	initialize_flags();

	glGenTextures(N_TEXTURES_USED, texture_names);
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
	prepare_optimus_20171665();

	prepare_sphere_20171665();
	prepare_cube_20171665();
	//set_lights();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
	initialize_camera();
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
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
