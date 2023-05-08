#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <algorithm>
#include <queue>
#include <tuple>
#include <vector>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0
#define TIME_STAMP_NUM 1000

#define CAR_NUM 53
#define HOUSE_NUM 20
#define PERSON_NUM 24
#define AIRPLANE_NUM 89
#define SHIRT_NUM 40
#define PEAK_NUM 30

#define BUMPED_MAX 20

using pp = std::pair<float, std::pair<int, char>>; // (y_val, (mat, type))

int win_width = 750.0f, win_height = 750.0f; 
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

bool car_bumped[CAR_NUM] = { false };
int car_bumped_time[CAR_NUM] = { 0 };
bool rider_car_bumped = false;
int rider_car_bumped_time = 0;
int rider_car_bumped_num = 0;
float rider_car_bumped_angle = 1.0f;


bool falling_weapon_fall[AIRPLANE_NUM] = { false };
float falling_weapon_explosion_time[AIRPLANE_NUM] = { 0.f };


GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

glm::mat4 road_right_line_cur = glm::mat4(1.0f);
glm::mat4 road_left_line_cur = glm::mat4(1.0f);

glm::mat4 road_cur[4] = { glm::mat4(1.0f) };
glm::mat4 rider_car_cur[4] = { glm::mat4(1.0f) };
glm::mat4 car_cur[CAR_NUM][4] = { glm::mat4(1.0f) };
glm::mat4 falling_weapon_cur[AIRPLANE_NUM][4] = {glm::mat4(1.0f) };
glm::mat4 house_cur[HOUSE_NUM][4] = { glm::mat4(1.0f) };
glm::mat4 person_cur[PERSON_NUM][4] = { glm::mat4(1.0f) };


unsigned int timestamp[TIME_STAMP_NUM];
void timer(int value) {
	for (int i = 0; i < TIME_STAMP_NUM; i++) {
		timestamp[i] = (timestamp[i] + 1) % UINT_MAX;
	}
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

void initialize_timestamp(int step) {
	timestamp[0] = 0;
	for (int i = 1; i < TIME_STAMP_NUM; i++) {
		timestamp[i] = (timestamp[i - 1] + step) % UINT_MAX;
	}
}

void prepare_axes(void) { // Draw axes in their MC.
	axes[0][0] = -win_width / 2.5f; axes[0][1] = 0.0f;
	axes[1][0] = win_width / 2.5f; axes[1][1] = 0.0f;
	axes[2][0] = 0.0f; axes[2][1] = -win_height / 2.5f;
	axes[3][0] = 0.0f; axes[3][1] = win_height / 2.5f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	axes[0][0] = -win_width / 2.5f; axes[1][0] = win_width / 2.5f; 
	axes[2][1] = -win_height / 2.5f;
	axes[3][1] = win_height / 2.5f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f; 
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f; 
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) { // Draw line in its MC.
	// y = x - win_height/4
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

// sky
#define SKY_WIDTH 1000.0f
#define SKY_HEIGHT 1000.0f

GLfloat sky[4][2] = {
    { -SKY_WIDTH / 2.0f, -SKY_HEIGHT / 2.0f }, 
    { SKY_WIDTH / 2.0f, -SKY_HEIGHT / 2.0f }, 
    { SKY_WIDTH / 2.0f, SKY_HEIGHT / 2.0f }, 
    { -SKY_WIDTH / 2.0f, SKY_HEIGHT / 2.0f } 
};

GLfloat sky_color[3] = { 92.0f / 255.0f, 245.0f / 255.0f, 233.0f / 255.0f };
GLuint VBO_sky, VAO_sky;

void prepare_sky(void) {
    // Initialize vertex buffer object.
    glGenBuffers(1, &VBO_sky);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sky);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sky), sky, GL_STATIC_DRAW);
    // Initialize vertex array object.
    glGenVertexArrays(1, &VAO_sky);
    glBindVertexArray(VAO_sky);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sky);
    glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void draw_sky(void) {
    glUniform3fv(loc_primitive_color, 1, sky_color);
    glBindVertexArray(VAO_sky);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

// ground
#define GROUND_WIDTH 1000.0f
#define GROUND_HEIGHT 1000.0f

GLfloat ground[4][2] = {
    { -GROUND_WIDTH / 2.0f, -GROUND_HEIGHT / 2.0f }, 
    { GROUND_WIDTH / 2.0f, -GROUND_HEIGHT / 2.0f }, 
    { GROUND_WIDTH / 2.0f, GROUND_HEIGHT / 2.0f }, 
    { -GROUND_WIDTH / 2.0f, GROUND_HEIGHT / 2.0f } 
};

GLfloat ground_color[3] = { 0.0f, 1.0f, 0.0f };
GLuint VBO_ground, VAO_ground;

void prepare_ground(void) {
    // Initialize vertex buffer object.
    glGenBuffers(1, &VBO_ground);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ground);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);
    // Initialize vertex array object.
    glGenVertexArrays(1, &VAO_ground);
    glBindVertexArray(VAO_ground);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ground);
    glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


// road
#define ROAD_WIDTH 100.0f
#define ROAD_LENGTH 800.0f
#define ROAD_PERSPECTIVE_OFFSET 200.0f
#define ROAD_LINE_INTERVAL 40.0f

GLfloat road[4][2] = { 
	{ -(ROAD_WIDTH + ROAD_PERSPECTIVE_OFFSET) / 2.0f, -ROAD_LENGTH / 2.0f }, 
	{ (ROAD_WIDTH + ROAD_PERSPECTIVE_OFFSET) / 2.0f, -ROAD_LENGTH / 2.0f }, 
	{ ROAD_WIDTH / 2.0f, ROAD_LENGTH / 2.0f }, 
	{ -ROAD_WIDTH / 2.0f, ROAD_LENGTH / 2.0f } 
};
GLfloat road_left_line[2][2] = {
	{ -(ROAD_WIDTH + ROAD_PERSPECTIVE_OFFSET) / 2.0f, -ROAD_LENGTH / 2.0f },
	{ -ROAD_WIDTH / 2.0f, ROAD_LENGTH / 2.0f }
};

GLfloat road_right_line[2][2] = {
	{ (ROAD_WIDTH + ROAD_PERSPECTIVE_OFFSET) / 2.0f, -ROAD_LENGTH / 2.0f },
	{ ROAD_WIDTH / 2.0f, ROAD_LENGTH / 2.0f }
};

GLfloat road_bbox[4][2] = {
	{ road_left_line[0][0], road_left_line[0][1] },
	{ road_left_line[1][0], road_left_line[1][1] },
	{ road_right_line[1][0], road_right_line[1][1] },
	{ road_right_line[0][0], road_right_line[0][1] }
};

GLfloat road_color[3] = { 0.0f, 0.0f, 0.0f };
GLfloat road_line_color[3] = { 1.0f, 1.0f, 1.0f };

GLuint VBO_road, VAO_road;

void prepare_road(void) {
	GLsizeiptr buffer_size = sizeof(road) 
		+ sizeof(road_left_line) + sizeof(road_right_line);
    
	// Initialize vertex buffer object.
    glGenBuffers(1, &VBO_road);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_road);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(road), road);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road), sizeof(road_left_line), road_left_line);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road) + sizeof(road_left_line), sizeof(road_right_line), road_right_line);

    // Initialize vertex array object.
    glGenVertexArrays(1, &VAO_road);
    glBindVertexArray(VAO_road);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_road);
    glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
	glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void draw_road() {
    glBindVertexArray(VAO_road);
	glUniform3fv(loc_primitive_color, 1, road_color);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, road_line_color);
	glLineWidth(6);
	glDrawArrays(GL_LINES, 4, 4);

    glBindVertexArray(0);
}


// road center
GLfloat road_center_line1[2][2] = {
	{ 0.0f, -ROAD_LENGTH + ROAD_LINE_INTERVAL },
	{ 0.0f, -3.0f * ROAD_LENGTH / 4.0f - ROAD_LINE_INTERVAL }
};

GLfloat road_center_line2[2][2] = {
	{ 0.0f, -3.0f * ROAD_LENGTH / 4.0f + ROAD_LINE_INTERVAL },
	{ 0.0f, -ROAD_LENGTH / 2.0f - ROAD_LINE_INTERVAL }
};

GLfloat road_center_line3[2][2] = {
	{ 0.0f, -ROAD_LENGTH / 2.0f + ROAD_LINE_INTERVAL },
	{ 0.0f, -ROAD_LENGTH / 4.0f - ROAD_LINE_INTERVAL}
};

GLfloat road_center_line4[2][2] = {
	{ 0.0f, -ROAD_LENGTH / 4.0f + ROAD_LINE_INTERVAL },
	{ 0.0f, 0.0f - ROAD_LINE_INTERVAL }
};

GLfloat road_center_line5[2][2] = {
	{ 0.0f, 0.0f + ROAD_LINE_INTERVAL },
	{ 0.0f, ROAD_LENGTH / 4.0f - ROAD_LINE_INTERVAL }
};

GLfloat road_center_line6[2][2] = {
	{ 0.0f, ROAD_LENGTH / 4.0f + ROAD_LINE_INTERVAL },
	{ 0.0f, ROAD_LENGTH / 2.0f - ROAD_LINE_INTERVAL }
};

GLfloat road_center_line7[2][2] = {
	{ 0.0f, ROAD_LENGTH / 2.0f + ROAD_LINE_INTERVAL },
	{ 0.0f, 3.0f * ROAD_LENGTH / 4.0f - ROAD_LINE_INTERVAL}
};

GLfloat road_center_line8[2][2] = {
	{ 0.0f, 3.0f * ROAD_LENGTH / 4.0f + ROAD_LINE_INTERVAL },
	{ 0.0f, ROAD_LENGTH - ROAD_LINE_INTERVAL }
};


GLfloat road_center_line_color[3] = { 1.0f, 1.0f, 0.0f };
GLuint VBO_road_center_line, VAO_road_center_line;

void prepare_road_center_line() {
	GLsizeiptr buffer_size = sizeof(road_center_line1) + sizeof(road_center_line2) 
		+ sizeof(road_center_line3) + sizeof(road_center_line4)
		+ sizeof(road_center_line5) + sizeof(road_center_line6)
		+ sizeof(road_center_line7) + sizeof(road_center_line8);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_road_center_line);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_road_center_line);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(road_center_line1), road_center_line1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road_center_line1), sizeof(road_center_line2), road_center_line2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road_center_line1) + sizeof(road_center_line2), sizeof(road_center_line3), road_center_line3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road_center_line1) + sizeof(road_center_line2) + sizeof(road_center_line3), sizeof(road_center_line4), road_center_line4);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road_center_line1) + sizeof(road_center_line2) + sizeof(road_center_line3) + sizeof(road_center_line4), sizeof(road_center_line5), road_center_line5);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road_center_line1) + sizeof(road_center_line2) + sizeof(road_center_line3) + sizeof(road_center_line4)
		+ sizeof(road_center_line5), sizeof(road_center_line6), road_center_line6);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road_center_line1) + sizeof(road_center_line2) + sizeof(road_center_line3) + sizeof(road_center_line4)
		+ sizeof(road_center_line5) + sizeof(road_center_line6), sizeof(road_center_line7), road_center_line7);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road_center_line1) + sizeof(road_center_line2) + sizeof(road_center_line3) + sizeof(road_center_line4)
		+ sizeof(road_center_line5) + sizeof(road_center_line6) + sizeof(road_center_line7), sizeof(road_center_line8), road_center_line8);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_road_center_line);
	glBindVertexArray(VAO_road_center_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_road_center_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
}

void draw_road_center_line() {
    glBindVertexArray(VAO_road_center_line);

    glUniform3fv(loc_primitive_color, 1, road_center_line_color);
    glLineWidth(4);
    glDrawArrays(GL_LINES, 0, 16);
    glLineWidth(1);

    glBindVertexArray(0);
}

//house
#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

GLfloat roof[3][2] = { { -12.0, 0.0 },{ 0.0, 12.0 },{ 12.0, 0.0 } };
GLfloat house_body[4][2] = { { -12.0, -14.0 },{ -12.0, 0.0 },{ 12.0, 0.0 },{ 12.0, -14.0 } };
GLfloat chimney[4][2] = { { 6.0, 6.0 },{ 6.0, 14.0 },{ 10.0, 14.0 },{ 10.0, 2.0 } };
GLfloat door[4][2] = { { -8.0, -14.0 },{ -8.0, -8.0 },{ -4.0, -8.0 },{ -4.0, -14.0 } };
GLfloat window[4][2] = { { 4.0, -6.0 },{ 4.0, -2.0 },{ 8.0, -2.0 },{ 8.0, -6.0 } };

GLfloat house_bbox[4][2] = {
    { -12.0, -14.0 },
    { -12.0, 14.0 },
    { 12.0, -14.0 },
    { 12.0, 14.0 }
};

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};

// random house roof color
GLfloat house_roof_color[3][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 125 / 255.0f, 215 / 255.0f, 196 / 255.0f },
	{ 235 / 255.0f, 195 / 255.0f, 0 / 255.0f }
};

// random house chimney color
GLfloat house_chimney_color[3][3] = {
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 33 / 255.0f, 133 / 255.0f, 213 / 255.0f },
	{ 144 / 255.0f, 150 / 255.0f, 49 / 255.0f }
};

GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door)
		+ sizeof(window);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door),
		sizeof(window), window);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house(int color_seed = 0) {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_roof_color[color_seed % 3]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_chimney_color[color_seed % 3]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}


// airplane
#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6
GLfloat big_wing[6][2] = { { 0.0, 0.0 }, { -20.0, 15.0 }, { -20.0, 20.0 }, { 0.0, 23.0 }, { 20.0, 20.0 }, { 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 }, { -11.0, -12.0 }, { -12.0, -7.0 }, { 0.0, -10.0 }, { 12.0, -7.0 }, { 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 }, { -6.0, 0.0 }, { -6.0, 22.0 }, { 6.0, 22.0 }, { 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 }, { -7.0, 24.0 }, { -7.0, 21.0 }, { 7.0, 21.0 }, { 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 }, { -18.0, 3.0 }, { -16.0, 10.0 }, { -18.0, 20.0 }, { -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 }, { 18.0, 3.0 }, { 16.0, 10.0 }, { 18.0, 20.0 }, { 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };
GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLfloat airplane_bbox[4][2] = {
    { -20.0, -25.0 }, 
	{ -20.0, 25.0 }, 
	{ 20.0, 25.0 }, 
	{ 20.0, -25.0 }
};

GLuint VBO_airplane, VAO_airplane;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1)+sizeof(sidewinder2)+sizeof(center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1)+sizeof(sidewinder2), sizeof(center), center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { // Draw airplane in its MC.
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}


// draw green peak
#define GREEN_PEAK 0
#define GREEN_PEAK2 1
#define GREEN_PEAK3 2
#define GREEN_PEAK4 3
#define GREEN_PEAK5 4

GLfloat green_peak_vertices[5][3][2] = {
    { { -1.5, 0.0 },{ -1.0, 0.5 },{ -0.5, 0.0 } },
    { { -1.0, 0.0 },{ -0.5, 1.0 },{ 0.0, 0.0 } },
    { { -0.5, 0.0 },{ 0.0, 0.75 },{ 0.5, 0.0 } },
    { { 0.0, 0.0 },{ 0.5, 1.0 },{ 1.0, 0.0 } },
    { { 0.5, 0.0 },{ 1.0, 0.5 },{ 1.5, 0.0 } }
};

GLfloat green_peak_color[3] = { 0.0f, 0.8f, 0.3f };

GLuint VBO_green_peak, VAO_green_peak;

void prepare_green_peak(void) {
    // Initialize vertex buffer object.
    glGenBuffers(1, &VBO_green_peak);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_green_peak);
    glBufferData(GL_ARRAY_BUFFER, sizeof(green_peak_vertices), green_peak_vertices, GL_STATIC_DRAW);
    // Initialize vertex array object.
    glGenVertexArrays(1, &VAO_green_peak);
    glBindVertexArray(VAO_green_peak);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_green_peak);
    glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void draw_green_peak(void) {
    glBindVertexArray(VAO_green_peak);
    glUniform3fv(loc_primitive_color, 1, green_peak_color);
	for (int i = 0; i < 5; i++) {
        glDrawArrays(GL_TRIANGLE_FAN, i * 3, 3);
    }
    glBindVertexArray(0);
}



// draw fire
#define EXPLOSION_RADIUS 20.0f

GLfloat explosion_color[3][3] = {
	{ 1.0f, 0.0f, 0.0f },  // red
	{ 1.0f, 0.5f, 0.0f },  // orange
	{ 1.0f, 1.0f, 0.0f }   // yellow
};

GLfloat explosion_vertices[3][360][2];
GLuint VBO_explosion, VAO_explosion;

void prepare_fire(void) {
	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_explosion);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_explosion);
	glBufferData(GL_ARRAY_BUFFER, sizeof(explosion_vertices), NULL, GL_STATIC_DRAW);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 360; j++) {
			GLfloat angle = j * 3.14159265358979323846 / 180.0f;

			if (j % 30 < 15) {
				GLfloat x = EXPLOSION_RADIUS * (i + 1 + (j % 15) / 15.0f) * cos(angle);
				GLfloat y = EXPLOSION_RADIUS * (i + 1 + (j % 15) / 15.0f) * sin(angle);
				explosion_vertices[i][j][0] = x;
				explosion_vertices[i][j][1] = y;
			}
			else {
				GLfloat x = EXPLOSION_RADIUS * (i + 1 + (15.0f - j % 15) / 15.0f) * cos(angle);
				GLfloat y = EXPLOSION_RADIUS * (i + 1 + (15.0f - j % 15) / 15.0f) * sin(angle);
				explosion_vertices[i][j][0] = x;
				explosion_vertices[i][j][1] = y;
			}
		}
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(explosion_vertices[0]), explosion_vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(explosion_vertices[0]), sizeof(explosion_vertices[1]), explosion_vertices[1]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(explosion_vertices[0]) + sizeof(explosion_vertices[1]), sizeof(explosion_vertices[2]), explosion_vertices[2]);
	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_explosion);
	glBindVertexArray(VAO_explosion);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_explosion);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_fire(void) {
	for (int i = 2; i >= 0; i--) {
		glUniform3fv(loc_primitive_color, 1, explosion_color[i]);
		glBindVertexArray(VAO_explosion);
		glDrawArrays(GL_TRIANGLE_FAN, 360 * i, 360);
		glBindVertexArray(0);
	}
}


// draw staminar bar
#define STAMINAR_BAR_WIDTH 100.0f
#define STAMINAR_BAR_HEIGHT 20.f

GLfloat staminar_bar_vertices[4][2] = {
    { -STAMINAR_BAR_WIDTH, -STAMINAR_BAR_HEIGHT },
    { STAMINAR_BAR_WIDTH, -STAMINAR_BAR_HEIGHT },
    { STAMINAR_BAR_WIDTH, STAMINAR_BAR_HEIGHT },
    { -STAMINAR_BAR_WIDTH, STAMINAR_BAR_HEIGHT }
};

GLuint VBO_staminar_bar, VAO_staminar_bar;


GLfloat staminar_bar_color[3] = { 1.0f, 0.0f, 1.0f };

void prepare_staminar_bar(void) {
    // Initialize vertex buffer object.
    glGenBuffers(1, &VBO_staminar_bar);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_staminar_bar);
    glBufferData(GL_ARRAY_BUFFER, sizeof(staminar_bar_vertices), staminar_bar_vertices, GL_STATIC_DRAW);
    // Initialize vertex array object.
    glGenVertexArrays(1, &VAO_staminar_bar);
    glBindVertexArray(VAO_staminar_bar);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_staminar_bar);
    glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void draw_staminar_bar(void) {
    glBindVertexArray(VAO_staminar_bar);
	glUniform3fv(loc_primitive_color, 1, staminar_bar_color);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}


// cake
#define CAKE_FIRE 0
#define CAKE_CANDLE 1
#define CAKE_BODY 2
#define CAKE_BOTTOM 3
#define CAKE_DECORATE 4

GLfloat cake_fire[4][2] = { { -0.5, 14.0 },{ -0.5, 13.0 },{ 0.5, 13.0 },{ 0.5, 14.0 } };
GLfloat cake_candle[4][2] = { { -1.0, 8.0 } ,{ -1.0, 13.0 },{ 1.0, 13.0 },{ 1.0, 8.0 } };
GLfloat cake_body[4][2] = { { 8.0, 5.0 },{ -8.0, 5.0 } ,{ -8.0, 8.0 },{ 8.0, 8.0 } };
GLfloat cake_bottom[4][2] = { { -10.0, 1.0 },{ -10.0, 5.0 },{ 10.0, 5.0 },{ 10.0, 1.0 } };
GLfloat cake_decorate[4][2] = { { -10.0, 0.0 },{ -10.0, 1.0 },{ 10.0, 1.0 },{ 10.0, 0.0 } };

GLfloat cake_color[5][3] = {
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 204 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 102 / 255.0f, 255 / 255.0f },
	{ 255 / 255.0f, 102 / 255.0f, 255 / 255.0f },
	{ 102 / 255.0f, 51 / 255.0f, 0 / 255.0f }
};

GLuint VBO_cake, VAO_cake;

void prepare_cake() {
	int size = sizeof(cake_fire);
	GLsizeiptr buffer_size = sizeof(cake_fire) * 5;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, size, cake_fire);
	glBufferSubData(GL_ARRAY_BUFFER, size, size, cake_candle);
	glBufferSubData(GL_ARRAY_BUFFER, size * 2, size, cake_body);
	glBufferSubData(GL_ARRAY_BUFFER, size * 3, size, cake_bottom);
	glBufferSubData(GL_ARRAY_BUFFER, size * 4, size, cake_decorate);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cake);
	glBindVertexArray(VAO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cake() {
	glBindVertexArray(VAO_cake);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_FIRE]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_CANDLE]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_DECORATE]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glBindVertexArray(0);
}


//shirt
#define SHIRT_LEFT_BODY 0
#define SHIRT_RIGHT_BODY 1
#define SHIRT_LEFT_COLLAR 2
#define SHIRT_RIGHT_COLLAR 3
#define SHIRT_FRONT_POCKET 4
#define SHIRT_BUTTON1 5
#define SHIRT_BUTTON2 6
#define SHIRT_BUTTON3 7
#define SHIRT_BUTTON4 8
GLfloat left_body[6][2] = { { 0.0, -9.0 },{ -8.0, -9.0 },{ -11.0, 8.0 },{ -6.0, 10.0 },{ -3.0, 7.0 },{ 0.0, 9.0 } };
GLfloat right_body[6][2] = { { 0.0, -9.0 },{ 0.0, 9.0 },{ 3.0, 7.0 },{ 6.0, 10.0 },{ 11.0, 8.0 },{ 8.0, -9.0 } };
GLfloat left_collar[4][2] = { { 0.0, 9.0 },{ -3.0, 7.0 },{ -6.0, 10.0 },{ -4.0, 11.0 } };
GLfloat right_collar[4][2] = { { 0.0, 9.0 },{ 4.0, 11.0 },{ 6.0, 10.0 },{ 3.0, 7.0 } };
GLfloat front_pocket[6][2] = { { 5.0, 0.0 },{ 4.0, 1.0 },{ 4.0, 3.0 },{ 7.0, 3.0 },{ 7.0, 1.0 },{ 6.0, 0.0 } };
GLfloat button1[3][2] = { { -1.0, 6.0 },{ 1.0, 6.0 },{ 0.0, 5.0 } };
GLfloat button2[3][2] = { { -1.0, 3.0 },{ 1.0, 3.0 },{ 0.0, 2.0 } };
GLfloat button3[3][2] = { { -1.0, 0.0 },{ 1.0, 0.0 },{ 0.0, -1.0 } };
GLfloat button4[3][2] = { { -1.0, -3.0 },{ 1.0, -3.0 },{ 0.0, -4.0 } };

GLfloat shirt_color[9][3] = {
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f }
};

GLuint VBO_shirt, VAO_shirt;
void prepare_shirt() {
	GLsizeiptr buffer_size = sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3) + sizeof(button4);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(left_body), left_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body), sizeof(right_body), right_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body), sizeof(left_collar), left_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar), sizeof(right_collar), right_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar),
		sizeof(front_pocket), front_pocket);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket), sizeof(button1), button1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1), sizeof(button2), button2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2), sizeof(button3), button3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3), sizeof(button4), button4);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_shirt);
	glBindVertexArray(VAO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_shirt() {
	glBindVertexArray(VAO_shirt);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_FRONT_POCKET]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON1]);
	glDrawArrays(GL_TRIANGLE_FAN, 26, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON2]);
	glDrawArrays(GL_TRIANGLE_FAN, 29, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON3]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON4]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);
	glBindVertexArray(0);
}

// sword
#define SWORD_BODY 0
#define SWORD_BODY2 1
#define SWORD_HEAD 2
#define SWORD_HEAD2 3
#define SWORD_IN 4
#define SWORD_DOWN 5
#define SWORD_BODY_IN 6

GLfloat sword_body[4][2] = { { -6.0, 0.0 },{ -6.0, -4.0 },{ 6.0, -4.0 },{ 6.0, 0.0 } };
GLfloat sword_body2[4][2] = { { -2.0, -4.0 },{ -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -4.0 } };
GLfloat sword_head[4][2] = { { -2.0, 0.0 },{ -2.0, 16.0 } ,{ 2.0, 16.0 },{ 2.0, 0.0 } };
GLfloat sword_head2[3][2] = { { -2.0, 16.0 },{ 0.0, 19.46 } ,{ 2.0, 16.0 } };
GLfloat sword_in[4][2] = { { -0.3, 0.7 },{ -0.3, 15.3 } ,{ 0.3, 15.3 },{ 0.3, 0.7 } };
GLfloat sword_down[4][2] = { { -2.0, -6.0 } ,{ 2.0, -6.0 },{ 4.0, -8.0 },{ -4.0, -8.0 } };
GLfloat sword_body_in[4][2] = { { 0.0, -1.0 } ,{ 1.0, -2.732 },{ 0.0, -4.464 },{ -1.0, -2.732 } };

GLfloat sword_color[7][3] = {
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
	{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
	{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

GLuint VBO_sword, VAO_sword;

void prepare_sword() {
	GLsizeiptr buffer_size = sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down) + sizeof(sword_body_in);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sword_body), sword_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body), sizeof(sword_body2), sword_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2), sizeof(sword_head), sword_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head), sizeof(sword_head2), sword_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2), sizeof(sword_in), sword_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in), sizeof(sword_down), sword_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down), sizeof(sword_body_in), sword_body_in);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_sword);
	glBindVertexArray(VAO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sword() {
	glBindVertexArray(VAO_sword);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}


//car
#define CAR_BODY 0
#define CAR_FRAME 1
#define CAR_WINDOW 2
#define CAR_LEFT_LIGHT 3
#define CAR_RIGHT_LIGHT 4
#define CAR_LEFT_WHEEL 5
#define CAR_RIGHT_WHEEL 6

GLfloat car_body[4][2] = { { -16.0, -8.0 },{ -16.0, 0.0 },{ 16.0, 0.0 },{ 16.0, -8.0 } };
GLfloat car_frame[4][2] = { { -10.0, 0.0 },{ -10.0, 10.0 },{ 10.0, 10.0 },{ 10.0, 0.0 } };
GLfloat car_window[4][2] = { { -8.0, 0.0 },{ -8.0, 8.0 },{ 8.0, 8.0 },{ 8.0, 0.0 } };
GLfloat car_left_light[4][2] = { { -9.0, -6.0 },{ -10.0, -5.0 },{ -9.0, -4.0 },{ -8.0, -5.0 } };
GLfloat car_right_light[4][2] = { { 9.0, -6.0 },{ 8.0, -5.0 },{ 9.0, -4.0 },{ 10.0, -5.0 } };
GLfloat car_left_wheel[4][2] = { { -10.0, -12.0 },{ -10.0, -8.0 },{ -6.0, -8.0 },{ -6.0, -12.0 } };
GLfloat car_right_wheel[4][2] = { { 6.0, -12.0 },{ 6.0, -8.0 },{ 10.0, -8.0 },{ 10.0, -12.0 } };

GLfloat car_bbox[4][2] = {
	{ -16.0, -12.0 },
	{ -16.0, 10.0 },
	{ 16.0, 10.0 },
	{ 16.0, -12.0 }
};

GLfloat car_color[7][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f }
};

// random car body color
GLfloat car_body_color[3][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 199 / 255.0f, 149 / 255.0f, 0.0 / 255.0f },
	{ 149 / 255.0f, 0 / 255.0f, 199 / 255.0f }
};

GLuint VBO_car, VAO_car;
void prepare_car() {
	GLsizeiptr buffer_size = sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel) + sizeof(car_right_wheel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car_body), car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body), sizeof(car_frame), car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame), sizeof(car_window), car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window), sizeof(car_left_light), car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light),
		sizeof(car_right_light), car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light), sizeof(car_left_wheel), car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel), sizeof(car_right_wheel), car_right_wheel);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car);
	glBindVertexArray(VAO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car(int color_seed = 0) {
	glBindVertexArray(VAO_car);

	glUniform3fv(loc_primitive_color, 1, car_body_color[color_seed % 3]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, car_body_color[color_seed % 3]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glBindVertexArray(0);
}


#define PERSON_HAT 0
#define PERSON_FACE 1
#define PERSON_EYE1 2
#define PERSON_EYE2 3
#define PERSON_MOUSE 4
#define PERSON_BODY 5
#define PERSON_FOOT1 6
#define PERSON_FOOT2 7
#define PERSON_ARM1 8
#define PERSON_ARM2 9

GLfloat person_hat[8][2] = { 
	{ -10.0f, 6.0f },
	{ -10.0f, 10.0f },
	{ -7.0f, 14.0f },
	{ -3.0f, 16.0f }, 
	{ 3.0f, 16.0f },
    { 7.0f, 14.0f },
	{ 10.0f, 10.0f },
	{ 10.0f, 6.0f } 
};
GLfloat person_face[4][2] = { 
	{ -10.0f, 6.0f },
	{ 10.0f, 6.0f },
	{ 10.0f, -8.0f },
	{ -10.0f, -8.0f } 
};
GLfloat person_eye1[6][2] = { 
	{ 3.0f, 0.0f },
	{ 5.0f, -2.0f },
	{ 6.0f, -2.0f },
	{ 4.0f, 0.0f },
	{ 6.0f, 2.0f },
    { 5.0f, 2.0f }
};
GLfloat person_eye2[6][2] = { 
	{ -3.0f, 0.0f },
	{ -5.0f, -2.0f },
	{ -6.0f, -2.0f },
	{ -4.0f, 0.0f },
	{ -6.0f, 2.0f },
	{ -5.0f, 2.0f }
};
GLfloat person_mouse[4][2] = { 
	{ -2.0f, -2.0f },
	{ -2.0f, -3.0f },
	{ 2.0f, -3.0f },
	{ 2.0f, -2.0f } 
};
GLfloat person_body[4][2] = {
    { -10.0f, -24.0f },
    { 10.0f, -24.0f },
    { 10.0f, -8.0f },
    { -10.0f, -8.0f } 
};
GLfloat person_foot1[6][2] = { 
	{ 4.0f, -26.0f },
	{ 14.0f, -26.0f },
	{ 14.0f, -24.0f },
	{ 12.0f, -20.0f },
	{ 6.0f, -20.0f },
	{ 4.0f, -24.0f } 
};
GLfloat person_foot2[6][2] = { 
	{ -4.0f, -24.0f },
	{ -14.0f, -24.0f },
	{ -14.0f, -22.0f },
	{ -12.0f, -18.0f },
	{ -6.0f, -18.0f },
	{ -4.0f, -22.0f } 
};
GLfloat person_arm1[4][2] = {

    { 10.0f, -8.0f },
    { 14.0f, -8.0f },
    { 14.0f, -16.0f },
    { 10.0f, -16.0f } 
};
GLfloat person_arm2[4][2] = {
    { -10.0f, -8.0f },
    { -14.0f, -8.0f },
    { -14.0f, -16.0f },
    { -10.0f, -16.0f } 
};

GLfloat person_bbox[4][2] = {
	{ -14.0f, -26.0f },
	{ 14.0f, -26.0f },
	{ 14.0f, 16.0f },
	{ -14.0f, 16.0f }
};

GLfloat person_color[10][3] = {
	{ 36.0f / 255.0f, 45.0f / 255.0f, 33.0f / 255.0f},
	{ 244.0f / 255.0f, 232.0f / 255.0f, 221.0f / 255.0f },
	{ 0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f },
	{ 0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f },
	{ 0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f },
	{ 216.0f / 255.0f, 135.0f / 255.0f, 186.0f / 255.0f },
	{ 118.0f / 255.0f, 85.0f / 255.0f, 81.0f / 255.0f },
	{ 118.0f / 255.0f, 85.0f / 255.0f, 81.0f / 255.0f },
	{ 244.0f / 255.0f, 232.0f / 255.0f, 221.0f / 255.0f },
	{ 244.0f / 255.0f, 232.0f / 255.0f, 221.0f / 255.0f }
};

// random body color
GLfloat person_body_color[3][3] = {
	{ 216.0f / 255.0f, 135.0f / 255.0f, 186.0f / 255.0f },
	{ 198.0f / 255.0f, 215.0f / 255.0f, 81.0f / 255.0f },
	{ 124.0f / 255.0f, 152.0f / 255.0f, 201.0f / 255.0f }
};

GLuint VBO_person, VAO_person;

void prepare_person() {
	GLsizeiptr buffer_size = sizeof(person_hat) + sizeof(person_face) + 
		sizeof(person_eye1) + sizeof(person_eye2) + 
		sizeof(person_mouse) + sizeof(person_body) + 
		sizeof(person_foot1) + sizeof(person_foot2) +
		sizeof(person_arm1) + sizeof(person_arm2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_person);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_person);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(person_hat), person_hat);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(person_hat), sizeof(person_face), person_face);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(person_hat) + sizeof(person_face), sizeof(person_eye1), person_eye1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(person_hat) + sizeof(person_face) + sizeof(person_eye1), sizeof(person_eye2), person_eye2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(person_hat) + sizeof(person_face) + sizeof(person_eye1) + sizeof(person_eye2), sizeof(person_mouse), person_mouse);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(person_hat) + sizeof(person_face) + sizeof(person_eye1) + sizeof(person_eye2) + sizeof(person_mouse), sizeof(person_body), person_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(person_hat) + sizeof(person_face) + sizeof(person_eye1) + sizeof(person_eye2) + sizeof(person_mouse) + sizeof(person_body), sizeof(person_foot1), person_foot1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(person_hat) + sizeof(person_face) + sizeof(person_eye1) + sizeof(person_eye2) + sizeof(person_mouse) + sizeof(person_body) + sizeof(person_foot1), sizeof(person_foot2), person_foot2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(person_hat) + sizeof(person_face) + sizeof(person_eye1) + sizeof(person_eye2) + sizeof(person_mouse) + sizeof(person_body)
		+ sizeof(person_foot1) + sizeof(person_foot2), sizeof(person_arm1), person_arm1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(person_hat) + sizeof(person_face) + sizeof(person_eye1) + sizeof(person_eye2) + sizeof(person_mouse) + sizeof(person_body)
		+ sizeof(person_foot1) + sizeof(person_foot2) + sizeof(person_arm1), sizeof(person_arm2), person_arm2);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_person);
	glBindVertexArray(VAO_person);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_person);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_person(int color_seed = 0) {
	glBindVertexArray(VAO_person);

	glUniform3fv(loc_primitive_color, 1, person_color[PERSON_HAT]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, person_color[PERSON_FACE]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, person_color[PERSON_EYE1]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 6);

	glUniform3fv(loc_primitive_color, 1, person_color[PERSON_EYE2]);
	glDrawArrays(GL_TRIANGLE_FAN, 18, 6);

	glUniform3fv(loc_primitive_color, 1, person_color[PERSON_MOUSE]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glUniform3fv(loc_primitive_color, 1, person_body_color[color_seed % 3]);
	glDrawArrays(GL_TRIANGLE_FAN, 28, 4);

	glUniform3fv(loc_primitive_color, 1, person_color[PERSON_FOOT1]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 6);

	glUniform3fv(loc_primitive_color, 1, person_color[PERSON_FOOT2]);
	glDrawArrays(GL_TRIANGLE_FAN, 38, 6);

	glUniform3fv(loc_primitive_color, 1, person_color[PERSON_ARM1]);
	glDrawArrays(GL_TRIANGLE_FAN, 44, 4);

	glUniform3fv(loc_primitive_color, 1, person_color[PERSON_ARM2]);
	glDrawArrays(GL_TRIANGLE_FAN, 44, 4);


	glBindVertexArray(0);
}


// rider car
#define RIDER_CAR_BODY 0
#define RIDER_CAR_FRAME 1
#define RIDER_CAR_WINDOW 2
#define RIDER_CAR_LEFT_LIGHT 3
#define RIDER_CAR_RIGHT_LIGHT 4
#define RIDER_CAR_LEFT_WHEEL 5
#define RIDER_CAR_RIGHT_WHEEL 6

#define	RIDER_PERSON_HAT 7
#define RIDER_PERSON_FACE 8
#define RIDER_PERSON_BODY 9
#define RIDER_PERSON_ARM1 10
#define RIDER_PERSON_ARM2 11

GLfloat rider_car_body[4][2] = { { -32.0, -16.0 },{ -32.0, 0.0 },{ 32.0, 0.0 },{ 32.0, -16.0 } };
GLfloat rider_car_frame[4][2] = { { -20.0, 0.0 },{ -20.0, 20.0 },{ 20.0, 20.0 },{ 20.0, 0.0 } };
GLfloat rider_car_window[4][2] = { { -16.0, 0.0 },{ -16.0, 16.0 },{ 16.0, 16.0 },{ 16.0, 0.0 } };
GLfloat rider_car_left_light[4][2] = { { -20.0, -10.0 },{ -20.0, -8.0 },{ -8.0, -8.0 },{ -8.0, -10.0 } };
GLfloat rider_car_right_light[4][2] = { { 20.0, -10.0 },{ 20.0, -8.0 },{ 8.0, -8.0 },{ 8.0, -10.0 } };
GLfloat rider_car_left_wheel[4][2] = { { -20.0, -24.0 },{ -20.0, -16.0 },{ -12.0, -16.0 },{ -12.0, -24.0 } };
GLfloat rider_car_right_wheel[4][2] = { { 12.0, -24.0 },{ 12.0, -16.0 },{ 20.0, -16.0 },{ 20.0, -24.0 } };

GLfloat rider_car_bbox[4][2] = {
	{-32.0f, -24.0f },
	{ -32.0f, 20.0f },
	{ 32.0f, 20.0f },
	{ 32.0f, -24.0f }
};

GLfloat rider_person_hat[8][2] = {
	{ -10.0f, 7.0f },
	{ -10.0f, 12.0f },
	{ -8.0f, 14.0f },
	{ -6.0f, 15.0f },
	{ -4.0f, 15.0f },
	{ -2.0f, 14.0f },
	{ 0.0f, 12.0f },
	{ 0.0f, 7.0f }
};

GLfloat rider_person_face[4][2] = {
	{ -10.0f, 7.0f },
	{ 0.0f, 7.0f },
	{ 0.0f, 4.0f },
	{ -10.0f, 4.0f }
};

GLfloat rider_person_body[4][2] = {
	{ -10.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 0.0f, 4.0f },
	{ -10.0f, 4.0f }
};

GLfloat rider_person_arm1[4][2] = {
	{ 0.0f, 4.0f },
	{ 2.0f, 4.0f },
	{ 2.0f, 1.0f },
	{ 0.0f, 1.0f }
};
GLfloat rider_person_arm2[4][2] = {
	{ -10.0f, 4.0f },
	{ -12.0f, 4.0f },
	{ -12.0f, 1.0f },
	{ -10.0f, 1.0f }
};

GLfloat rider_car_color[12][3] = {
	// car
	{ 135 / 255.0f, 49 / 255.0f, 59 / 255.0f },
	{ 135 / 255.0f, 49 / 255.0f, 59 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },

	// person
	{ 36.0f / 255.0f, 45.0f / 255.0f, 33.0f / 255.0f},
	{ 244.0f / 255.0f, 232.0f / 255.0f, 221.0f / 255.0f },
	{ 116.0f / 255.0f, 105.0f / 255.0f, 196.0f / 255.0f },
	{ 118.0f / 255.0f, 85.0f / 255.0f, 81.0f / 255.0f },
	{ 118.0f / 255.0f, 85.0f / 255.0f, 81.0f / 255.0f },
};


GLuint VBO_rider_car, VAO_rider_car;
void prepare_rider_car() {
	GLsizeiptr buffer_size = sizeof(rider_car_body) + sizeof(rider_car_frame) + sizeof(rider_car_window) + sizeof(rider_car_left_light)
		+ sizeof(rider_car_right_light) + sizeof(rider_car_left_wheel) + sizeof(rider_car_right_wheel) + sizeof(rider_person_hat) 
		+ sizeof(rider_person_face) + sizeof(rider_person_body) + sizeof(rider_person_arm1) + sizeof(rider_person_arm2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_rider_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_rider_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rider_car_body), rider_car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(rider_car_body), sizeof(rider_car_frame), rider_car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(rider_car_body) + sizeof(rider_car_frame), sizeof(rider_car_window), rider_car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(rider_car_body) + sizeof(rider_car_frame) + sizeof(rider_car_window), sizeof(rider_car_left_light), rider_car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(rider_car_body) + sizeof(rider_car_frame) + sizeof(rider_car_window) + sizeof(rider_car_left_light),
		sizeof(rider_car_right_light), rider_car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(rider_car_body) + sizeof(rider_car_frame) + sizeof(rider_car_window) + sizeof(rider_car_left_light)
		+ sizeof(rider_car_right_light), sizeof(rider_car_left_wheel), rider_car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(rider_car_body) + sizeof(rider_car_frame) + sizeof(rider_car_window) + sizeof(rider_car_left_light)
		+ sizeof(rider_car_right_light) + sizeof(rider_car_left_wheel), sizeof(rider_car_right_wheel), rider_car_right_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(rider_car_body) + sizeof(rider_car_frame) + sizeof(rider_car_window) + sizeof(rider_car_left_light)
	+ sizeof(rider_car_right_light) + sizeof(rider_car_left_wheel) + sizeof(rider_car_right_wheel), sizeof(rider_person_hat), rider_person_hat);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(rider_car_body) + sizeof(rider_car_frame) + sizeof(rider_car_window) + sizeof(rider_car_left_light)
	+ sizeof(rider_car_right_light) + sizeof(rider_car_left_wheel) + sizeof(rider_car_right_wheel) + sizeof(rider_person_hat), sizeof(rider_person_face), rider_person_face);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(rider_car_body) + sizeof(rider_car_frame) + sizeof(rider_car_window) + sizeof(rider_car_left_light)
	+ sizeof(rider_car_right_light) + sizeof(rider_car_left_wheel) + sizeof(rider_car_right_wheel) + sizeof(rider_person_hat) + sizeof(rider_person_face), sizeof(rider_person_body), rider_person_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(rider_car_body) + sizeof(rider_car_frame) + sizeof(rider_car_window) + sizeof(rider_car_left_light)
	+ sizeof(rider_car_right_light) + sizeof(rider_car_left_wheel) + sizeof(rider_car_right_wheel) + sizeof(rider_person_hat) + sizeof(rider_person_face) + sizeof(rider_person_body), sizeof(rider_person_arm1), rider_person_arm1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(rider_car_body) + sizeof(rider_car_frame) + sizeof(rider_car_window) + sizeof(rider_car_left_light)
	+ sizeof(rider_car_right_light) + sizeof(rider_car_left_wheel) + sizeof(rider_car_right_wheel) + sizeof(rider_person_hat) + sizeof(rider_person_face) + sizeof(rider_person_body) + sizeof(rider_person_arm1), sizeof(rider_person_arm2), rider_person_arm2);


	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_rider_car);
	glBindVertexArray(VAO_rider_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_rider_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_rider_car() {
	glBindVertexArray(VAO_rider_car);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_CAR_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_CAR_FRAME]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_CAR_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_CAR_LEFT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_CAR_RIGHT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_CAR_LEFT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_CAR_RIGHT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_PERSON_HAT]);
	glDrawArrays(GL_TRIANGLE_FAN, 28, 8);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_PERSON_FACE]);
	glDrawArrays(GL_TRIANGLE_FAN, 36, 4);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_PERSON_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 40, 4);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_PERSON_ARM1]);
	glDrawArrays(GL_TRIANGLE_FAN, 44, 4);

	glUniform3fv(loc_primitive_color, 1, rider_car_color[RIDER_PERSON_ARM2]);
	glDrawArrays(GL_TRIANGLE_FAN, 48, 4);

	glBindVertexArray(0);
}



void display(void) {
	int i;
	float x, r, s, delx, delr, dels;
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);
	srand(32223);

	std::priority_queue<pp, std::vector<pp>, std::less<pp>> draw_queue;

	// draw road
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -win_height / 4.0f, 0.0f));
	if (win_width > 400) {
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(win_width / (4.0f * ROAD_WIDTH), win_height / (2.0f * ROAD_LENGTH), 1.0f));
	}
	else {
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f, win_height / (2.0f * ROAD_LENGTH), 1.0f));
	}

	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_road();


	for (int i = 0; i < 4; i++) {
		road_cur[i][3][0] = road_bbox[i][0];
		road_cur[i][3][1] = road_bbox[i][1];
		road_cur[i][3][2] = 0.0f;
		road_cur[i][3][3] = 1.0f;

		road_cur[i] = ModelMatrix * road_cur[i];
	}

	int road_center_timestamp = timestamp[0] % (win_height / 1);

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -road_center_timestamp * 1.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f, win_height / ROAD_LENGTH, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_road_center_line();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, win_height / 4.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(win_width / (1.0f * SKY_WIDTH), win_height / (2.0f * SKY_HEIGHT), 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_sky();


	// draw green peak
	for (int i = 0; i < PEAK_NUM; i++) {
		float scaling_factor = 100.0f * (timestamp[i] % win_width) / win_width;
		if (i % 2) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-win_width / 4 -(int(timestamp[i] / 2) % (win_width)), 0.0f, 0.0f));
		}
		else {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(win_width / 4 + int(timestamp[i] / 2) % (win_width), 0.0f, 0.0f));
		}
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor, scaling_factor, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_green_peak();
	}

	// draw shirt
	int rotation_interval = 50;
	float shirt_pos[SHIRT_NUM];
	float shirt_clock[SHIRT_NUM];
	float shirt_pos_low = 0.3f;
	float shirt_pos_high = 1.8f;

	for (int i = 0; i < SHIRT_NUM; i++) {
		shirt_clock[i] = (timestamp[i]) % (win_width * 2);
		shirt_pos[i] = shirt_clock[i] * 2.0f;
		float shirt_var = shirt_pos_low + (shirt_pos_high - shirt_pos_low) * ((float)rand() / RAND_MAX);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(shirt_pos[i] + win_width / 8 * (1.5 - shirt_var), (20.0f * shirt_var) * sqrtf(shirt_pos[i]) + 10.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, atanf((20.0f * shirt_var) / (2.0f * sqrtf(shirt_pos[i]))), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f + shirt_pos[i] / win_width, 0.5f + shirt_pos[i] / win_width, 1.0f));

		if (timestamp[0] % 200 < 100) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0 + (timestamp[0] % 100) / 100.0f));
		}
		else {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0 - (timestamp[0] % 100) / 100.0f));
		}
		if (timestamp[0] % (rotation_interval * 4) < rotation_interval) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f * (timestamp[0] % rotation_interval) / rotation_interval, 1.0f, 1.0f));
		}
		else if (timestamp[0] % (rotation_interval * 4) < rotation_interval * 2) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f * (rotation_interval - timestamp[0] % rotation_interval) / rotation_interval, 1.0f, 1.0f));
		}
		else if (timestamp[0] % (rotation_interval * 4) < rotation_interval * 3) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(-1.0f * (timestamp[0] % rotation_interval) / rotation_interval, 1.0f, 1.0f));
		}
		else {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(-1.0f * (rotation_interval - timestamp[0] % rotation_interval) / rotation_interval, 1.0f, 1.0f));
		}
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_shirt();
	}


	for (int i = 0; i < SHIRT_NUM; i++) {
		shirt_clock[i] = (timestamp[i]) % (win_width * 2);
		shirt_pos[i] = shirt_clock[i] * 2.0f;
		float shirt_var = shirt_pos_low + (shirt_pos_high - shirt_pos_low) * ((float)rand() / RAND_MAX);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-shirt_pos[i] - win_width / 8 * (1.5 - shirt_var), (20.0f * shirt_var) * sqrtf(shirt_pos[i]) + 10.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, atanf(-(20.0f * shirt_var) / (2.0f * sqrtf(shirt_pos[i]))), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f + shirt_pos[i] / win_width, 0.5f + shirt_pos[i] / win_width, 1.0f));

		if (timestamp[0] % 200 < 100) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0 + (timestamp[0] % 100) / 100.0f));
		}
		else {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0 - (timestamp[0] % 100) / 100.0f));
		}
		if (timestamp[0] % (rotation_interval * 4) < rotation_interval) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f * (timestamp[0] % rotation_interval) / rotation_interval, 1.0f, 1.0f));
		}
		else if (timestamp[0] % (rotation_interval * 4) < rotation_interval * 2) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f * (rotation_interval - timestamp[0] % rotation_interval) / rotation_interval, 1.0f, 1.0f));
		}
		else if (timestamp[0] % (rotation_interval * 4) < rotation_interval * 3) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(-1.0f * (timestamp[0] % rotation_interval) / rotation_interval, 1.0f, 1.0f));
		}
		else {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(-1.0f * (rotation_interval - timestamp[0] % rotation_interval) / rotation_interval, 1.0f, 1.0f));
		}
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_shirt();
	}


	// draw airplane
	float airplane_pos[AIRPLANE_NUM];
	float airplane_clock[AIRPLANE_NUM];
	float airplane_pos_low = 0.3f;
	float airplane_pos_high = 1.8f;

	for (int i = 0; i < AIRPLANE_NUM; i++) {
		airplane_clock[i] = (timestamp[i]) % (win_width * 2);
		airplane_pos[i] = airplane_clock[i] * 2.0f;
		float airplane_var = airplane_pos_low + (airplane_pos_high - airplane_pos_low) * ((float)rand() / RAND_MAX);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_pos[i] + win_width / 8 * (1.5 - airplane_var), (20.0f * airplane_var) * sqrtf(airplane_pos[i]) + 10.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, atanf((20.0f * airplane_var) / (2.0f * sqrtf(airplane_pos[i]))), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f + airplane_pos[i] / win_width, 0.5f + airplane_pos[i] / win_width, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();
	}

	for (int i = 0; i < AIRPLANE_NUM; i++) {
		airplane_clock[i] = (timestamp[i]) % (win_width * 2);
		airplane_pos[i] = airplane_clock[i] * 2.0f;
		float airplane_var = airplane_pos_low + (airplane_pos_high - airplane_pos_low) * ((float)rand() / RAND_MAX);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-airplane_pos[i] - win_width / 8 * (1.5 - airplane_var), (20.0f * airplane_var) * sqrtf(airplane_pos[i]) + 10.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, atanf(-(20.0f * airplane_var) / (2.0f * sqrtf(airplane_pos[i]))), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f + airplane_pos[i] / win_width, 0.5f + airplane_pos[i] / win_width, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();
	}



	// draw cake
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	if (timestamp[0] % 100 >= 80 && timestamp[0] % 100 < 90) {
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0 - (timestamp[0] % 10) / 20.0f, 1.0 - (timestamp[0] % 10) / 20.0f, 1.0f));
	}
	else if (timestamp[0] % 100 >= 90) {
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5 + (timestamp[0] % 10) / 20.0f, 1.0 + (timestamp[0] % 10) / 20.0f, 1.0f));
	}
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(4.0f, 4.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_cake();




	// draw house
	std::vector<glm::mat4> house_model_matrix(HOUSE_NUM);

	float house_pos_low = 1.0f;
	float house_pos_high = 2.0f;
	float house_pos[HOUSE_NUM];
	float house_speed[HOUSE_NUM];
	float house_clock[HOUSE_NUM];

	for (int i = 0; i < HOUSE_NUM; i++) {
		house_speed[i] = 0.20f;
		house_pos[i] = house_pos_low + (house_pos_high - house_pos_low) * ((float)rand() / RAND_MAX);
		house_clock[i] = (timestamp[i]) % (HOUSE_NUM * 83);

		if (i % 2) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
				win_width / 4.0f * house_pos[i] + house_clock[i] * win_width / win_height * house_speed[i],
				5.0f - 5.0f * win_height / (4.0f * win_width) * house_clock[i] * win_width / win_height * house_speed[i],
				0.0f
			));
			ModelMatrix = glm::rotate(ModelMatrix, -atanf(1.0 / 1.1f), glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.1f, 1.0f, 1.0f));
		}
		else {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
				-win_width / 4.0f * house_pos[i] - house_clock[i] * win_width / win_height * house_speed[i],
				5.0f - 5.0f * win_height / (4.0f * win_width) * house_clock[i] * win_width / win_height * house_speed[i],
				0.0f
			));
			ModelMatrix = glm::rotate(ModelMatrix, -atanf(1.0 / -1.1f), glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(-1.1f, 1.0f, 1.0f));
		}

		ModelMatrix = glm::rotate(ModelMatrix, 45.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

		if (house_clock[i] < 100.0f) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(
				house_clock[i] / 100.0f,
				house_clock[i] / 100.0f,
				1.0f
			));
		}
		else {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(
				1.0f + (house_clock[i] - 100.0f) / 400.0f,
				1.0f + (house_clock[i] - 100.0f) / 400.0f,
				1.0f
			));
		}

		for (int j = 0; j < 4; j++) {
			house_cur[i][j][3][0] = house_bbox[j][0];
			house_cur[i][j][3][1] = house_bbox[j][1];
			house_cur[i][j][3][2] = 0.0f;
			house_cur[i][j][3][3] = 1.0f;

			house_cur[i][j] = ModelMatrix * house_cur[i][j];
		}

		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, house_cur[i][3][3][1]));
		/*ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();*/
		
		house_model_matrix[i] = ModelMatrix;
		draw_queue.push({ house_cur[i][3][3][1], {i, 'h'}});
	}


	// draw falling weapon
	std::vector<glm::mat4> falling_weapon_model_matrix(AIRPLANE_NUM);
	float falling_weapon_pos_low = -1.0f;
	float falling_weapon_pos_high = 1.0f;
	float falling_weapon_dist_low = 0.2f;
	float falling_weapon_dist_high = 1.0f;
	float falling_weapon_dumped_offset = 20.0f;
	float falling_weapon_var[AIRPLANE_NUM];
	float falling_weapon_pos[AIRPLANE_NUM];
	float falling_weapon_dist[AIRPLANE_NUM];
	float falling_weapon_clock[AIRPLANE_NUM];

	srand(32223);

	for (int i = 0; i < AIRPLANE_NUM; i++) {
		falling_weapon_pos[i] = falling_weapon_pos_low + (falling_weapon_pos_high - falling_weapon_pos_low) * ((float)rand() / RAND_MAX);
		falling_weapon_dist[i] = falling_weapon_dist_low + (falling_weapon_dist_high - falling_weapon_dist_low) * ((float)rand() / RAND_MAX);
		falling_weapon_var[i] = rand() % (win_height / 3);
		falling_weapon_clock[i] = (timestamp[i]) % (AIRPLANE_NUM * 29);


		float road_upper_length = road_cur[2][3][0] - road_cur[1][3][0];
		float road_lower_length = road_cur[3][3][0] - road_cur[0][3][0];

		float x_val = 0.0f;
		float y_val = -win_height / 2 * falling_weapon_dist[i];
		if (falling_weapon_pos[i] > 0) {
			x_val = road_lower_length / 2 * falling_weapon_pos[i] + (road_upper_length - road_lower_length) / 2 * falling_weapon_dist[i];

		}
		else {
			x_val = road_lower_length / 2 * falling_weapon_pos[i] - (road_upper_length - road_lower_length) / 2 * falling_weapon_dist[i];
		}

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
			x_val,
			win_height + falling_weapon_var[i] - falling_weapon_clock[i] * falling_weapon_clock[i] / 100.0f,
			0.0f)
		);

		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(
			(falling_weapon_dist[i] + 0.3) * 1.5f
		));

		for (int j = 0; j < 4; j++) {
			falling_weapon_cur[i][j][3][0] = airplane_bbox[j][0];
			falling_weapon_cur[i][j][3][1] = airplane_bbox[j][1];
			falling_weapon_cur[i][j][3][2] = 0.0f;
			falling_weapon_cur[i][j][3][3] = 1.0f;
			falling_weapon_cur[i][j] = ModelMatrix * falling_weapon_cur[i][j];
		}

		if (falling_weapon_cur[i][3][3][1] >= -win_height / 2 * falling_weapon_dist[i]) {
			if (falling_weapon_explosion_time[i]) {
				falling_weapon_explosion_time[i] = 0;
			}
			/*ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_airplane();*/

			falling_weapon_fall[i] = true;
			falling_weapon_model_matrix[i] = ModelMatrix;
			draw_queue.push({ falling_weapon_cur[i][3][3][1], {i, 'f' } });
		}
		else {
			if (falling_weapon_fall[i]){
				if (falling_weapon_explosion_time[i] < 30.0f) {
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
						x_val,
						-win_height / 2 * falling_weapon_dist[i],
						0.0f)
					);
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(
						(falling_weapon_dist[i] + 0.3) * falling_weapon_explosion_time[i] / 20.0f
					));
					falling_weapon_explosion_time[i] += 1.0f;
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_fire();

					for (int k = 0; k < CAR_NUM; k++) {
						if (car_bumped[k]) continue;
						if (car_cur[k][0][3][0] - falling_weapon_dumped_offset <= x_val && x_val <= car_cur[k][3][3][0] + falling_weapon_dumped_offset &&
							car_cur[k][0][3][1] - falling_weapon_dumped_offset <= y_val && y_val <= car_cur[k][1][3][1] + falling_weapon_dumped_offset) {
							car_bumped[k] = true;
						}
						
					}
					if (rider_car_cur[0][3][0] - falling_weapon_dumped_offset <= x_val && x_val <= rider_car_cur[3][3][0] + falling_weapon_dumped_offset &&
						rider_car_cur[0][3][1] - falling_weapon_dumped_offset <= y_val && y_val <= rider_car_cur[1][3][1] + falling_weapon_dumped_offset) {
						rider_car_bumped = true;
					}
				}
				else {
					falling_weapon_fall[i] = false;
				}
			}
			
		}

	}


	// draw car
	std::vector<glm::mat4> car_model_matrix(CAR_NUM);

	int min_val = (win_height > win_width) ? win_height : win_width;
	float car_speed_low = 0.4f;
	float car_speed_high = 0.45f;
	float car_pos_low = 0.1f;
	float car_pos_high = 2.3f;
	float car_pos[CAR_NUM];
	float car_speed[CAR_NUM];
	float car_clock[CAR_NUM];
	//glm::mat4 car_transform_matrix[CAR_NUM];

	for (int i = 0; i < CAR_NUM; i++) {
		car_speed[i] = car_speed_low + (car_speed_high - car_speed_low) * ((float)rand() / RAND_MAX);
		car_pos[i] = car_pos_low + (car_pos_high - car_pos_low) * ((float)rand() / RAND_MAX);
		car_clock[i] = (timestamp[i]) % (CAR_NUM * 63);
		if (i % 2) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
				win_width / (16 + car_pos[i] * 2.0f) * car_pos[i] + car_clock[i] * win_width / win_height * car_speed[i],
				20.0f - 4.0f * win_height / win_width * car_clock[i] * win_width / win_height * car_speed[i],
				0.0f
			));
		}
		else {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
				-win_width / (16 + car_pos[i] * 2.0f) * car_pos[i] - car_clock[i] * win_width / win_height * car_speed[i],
				20.0f - 4.0f * win_height / win_width * car_clock[i] * win_width / win_height * car_speed[i], 
				0.0f
			));
		}
		if (!car_bumped[i]) {
			if ((int)car_clock[i] % 2) {
				ModelMatrix = glm::rotate(ModelMatrix, -atanf(1.0 / 1.02f), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.02f, 1.0f, 1.0f));
			}
			else {
				ModelMatrix = glm::rotate(ModelMatrix, -atanf(1.0 / -1.02f), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(-1.02f, 1.0f, 1.0f));
			}
			ModelMatrix = glm::rotate(ModelMatrix, 45.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		if (car_clock[i] < 100.0f) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(
				car_clock[i] / 50.0f, 
				car_clock[i] / 50.0f,
				1.0f
			));
		}
		else {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(
				2.0f + (car_clock[i] - 100.0f) / 50.0f,
				2.0f + (car_clock[i] - 100.0f) / 50.0f,
				1.0f
			));
		}

		if (car_bumped[i]) {
			if (i % 2 == 0) {
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-car_bumped_time[i], 5.0f * sqrtf(car_bumped_time[i]), 0.0f));
			}
			else {
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(car_bumped_time[i], 5.0f * sqrtf(car_bumped_time[i]), 0.0f));
			}
			ModelMatrix = glm::rotate(ModelMatrix, (1.0f - (i % 2) * 2.0f) * car_bumped_time[i] * 2.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			car_bumped_time[i] += 1;
		}

		

		for (int j = 0; j < 4; j++) {
			car_cur[i][j][3][0] = car_bbox[j][0];
			car_cur[i][j][3][1] = car_bbox[j][1];
			car_cur[i][j][3][2] = 0.0f;
			car_cur[i][j][3][3] = 1.0f;
			car_cur[i][j] = ModelMatrix * car_cur[i][j];
		}

		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, -car_cur[i][3][3][1]));
		car_model_matrix[i] = ModelMatrix;

		// if car_cur is outside of window
		if (car_bumped[i]) {
			float max_val = (win_height > win_width) ? win_height : win_width;
			max_val *= 1.4f;
			if ((car_cur[i][0][3][0] < -max_val && car_cur[i][1][3][0] < -max_val * 1.5f &&
				car_cur[i][2][3][0] < -max_val && car_cur[i][3][3][0] < -max_val * 1.5f) ||
				(car_cur[i][0][3][0] > max_val && car_cur[i][1][3][0] > max_val * 1.5f &&
				car_cur[i][2][3][0] > max_val && car_cur[i][3][3][0] > max_val * 1.5f)) {
				car_bumped[i] = false;
				car_bumped_time[i] = 0;
			}
			draw_queue.push({ -float(win_height), {i, 'c' } });
		}
		else {
			draw_queue.push({ car_cur[i][3][3][1], {i, 'c' } });
		}

		/*ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car();
		car_model_matrix[i] = ModelMatrix;*/

		draw_queue.push({ car_cur[i][3][3][1], {i, 'c' } });
	}

	// draw person
	std::vector<glm::mat4> person_model_matrix(PERSON_NUM);

	float person_speed_low = 0.3f;
	float person_speed_high = 0.35f;
	float person_pos_low = 1.0f;
	float person_pos_high = 1.3f;
	float person_pos[PERSON_NUM];
	float person_speed[PERSON_NUM];
	float person_clock[PERSON_NUM];

	for (int i = PERSON_NUM - 1; i >= 0; i--) {
		person_speed[i] = person_speed_low + (person_speed_high - person_speed_low) * ((float)rand() / RAND_MAX);
		person_pos[i] = person_pos_low + (person_pos_high - person_pos_low) * ((float)rand() / RAND_MAX);
		person_clock[i] = (timestamp[i]) % (PERSON_NUM * 83);
		if (i % 2) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
				win_width / 8.0f * person_pos[i] + person_clock[i] * win_width / win_height * person_speed[i],
				60.0f - 3.0f * win_height / (2.0 * win_width) * person_clock[i] * win_width / win_height * person_speed[i],
				0.0f
			));
		}
		else {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
				-win_width / 8.0f * person_pos[i] - person_clock[i] * win_width / win_height * person_speed[i],
				60.0f - 3.0f * win_height / (2.0 * win_width) * person_clock[i] * win_width / win_height * person_speed[i],
				0.0f
			));
		}
		if ((int)person_clock[i] % 2) {
			ModelMatrix = glm::rotate(ModelMatrix, -atanf(1.0 / 1.02f), glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.02f, 1.0f, 1.0f));
		}
		else {
			ModelMatrix = glm::rotate(ModelMatrix, -atanf(1.0 / -1.02f), glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(-1.02f, 1.0f, 1.0f));
		}
		ModelMatrix = glm::rotate(ModelMatrix, 45.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

		if (person_clock[i] < 100.0f) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(
				person_clock[i] / 10000.0f,
				person_clock[i] / 10000.0f,
				1.0f
			));
		}
		else {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(
				0.01f + (person_clock[i] - 100.0f) / 400.0f,
				0.01f + (person_clock[i] - 100.0f) / 400.0f,
				1.0f
			));
		}

		for (int j = 0; j < 4; j++) {
			person_cur[i][j][3][0] = person_bbox[j][0];
			person_cur[i][j][3][1] = person_bbox[j][1];
			person_cur[i][j][3][2] = 0.0f;
			person_cur[i][j][3][3] = 1.0f;

			person_cur[i][j] = ModelMatrix * person_cur[i][j];
		}

		/*ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_person();*/

		person_model_matrix[i] = ModelMatrix;

		draw_queue.push({ person_cur[i][3][3][1], {i, 'p' } });
	}

	

	// draw rider car
	std::vector<glm::mat4> rider_car_model_matrix(1);

	if (timestamp[0] % 2) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f - (timestamp[0] % 4) / 2.0f, -win_height / 2 + 80.0f, 0.0f));
	}
	else {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -win_height / 2 + 80.0f, 0.0f));
	}

	if (timestamp[1] % 2) {
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.5f - (timestamp[1] % 4) / 2.0f, 0.0f));
	}
	else {
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	}


	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(centerx * 5.0f, 0.0f, 0.0f));

	float min_size = (2.5f * win_width / min_val > 2.0f) ? 2.5 * win_width / min_val : 2.0f;
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(min_size, min_size, 1.0f));

	for (int i = 0; i < 4; i++) {
		rider_car_cur[i][3][0] = rider_car_bbox[i][0];
		rider_car_cur[i][3][1] = rider_car_bbox[i][1];
		rider_car_cur[i][3][2] = 0.0f;
		rider_car_cur[i][3][3] = 1.0f;
		rider_car_cur[i] = ModelMatrix * rider_car_cur[i];
	}
	
	// check if a rider car is bumped into an another car
	for (int i = 0; i < CAR_NUM; i++) {
		if (car_bumped[i]) continue;
		// calculate the intersection of rider_car_cur and car_cur[i]
		float iou_x1, iou_x2, iou_y1, iou_y2;
		if (car_cur[i][3][3][1] >= rider_car_cur[3][3][1] - (car_cur[i][1][3][1] - car_cur[i][0][3][1]) / 2.0f && car_cur[i][3][3][1] <= rider_car_cur[1][3][1]) {
			iou_x1 = (rider_car_cur[1][3][0] > car_cur[i][1][3][0]) ? rider_car_cur[1][3][0] : car_cur[i][1][3][0];	// max(rider_car_cur[0][3][0], car_cur[i][0][3][0])
			iou_x2 = (rider_car_cur[3][3][0] < car_cur[i][3][3][0]) ? rider_car_cur[3][3][0] : car_cur[i][3][3][0];	// min(rider_car_cur[3][3][0], car_cur[i][3][3][0])
			iou_y1 = (rider_car_cur[1][3][1] > car_cur[i][1][3][1]) ? rider_car_cur[1][3][1] : car_cur[i][1][3][1];	// max(rider_car_cur[0][3][1], car_cur[i][0][3][1])
			iou_y2 = (rider_car_cur[3][3][1] < car_cur[i][3][3][1]) ? rider_car_cur[3][3][1] : car_cur[i][3][3][1];	// min(rider_car_cur[3][3][1], car_cur[i][3][3][1])


			if (iou_x1 < iou_x2 && iou_y1 > iou_y2) {
				// calculate the area of intersection
				float iou_area = (iou_x2 - iou_x1) * (iou_y1 - iou_y2);
				// calculate the area of union
				float iou_union = (rider_car_cur[3][3][0] - rider_car_cur[0][3][0]) * (rider_car_cur[1][3][1] - rider_car_cur[0][3][1]) + (car_cur[i][3][3][0] - car_cur[i][0][3][0]) * (car_cur[i][1][3][1] - car_cur[i][0][3][1]) - iou_area;
				// calculate the intersection over union
				float iou = iou_area / iou_union;
				if (iou > 0.5f && iou_area <= iou_union) {
					// collision detected
					// reset the position of rider car
					if (!rider_car_bumped) {
						rider_car_bumped_time = 0;
						rider_car_bumped = true;
						car_bumped[i] = true;
					}
				}
			}
		}
	}

	if (rider_car_bumped && rider_car_bumped_time <= 60) {
		float angle = rider_car_bumped_angle * rider_car_bumped_time * 6.0f * TO_RADIAN;
		if (rider_car_bumped_time == 0) {
			rider_car_bumped_angle = -1.0f;
			if (centerx < 0) {
				rider_car_bumped_angle = 1.0f;
			}
			rider_car_bumped_num += 1;
		}
		rider_car_bumped_time += 1;
		ModelMatrix = glm::rotate(ModelMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		
	
		if (rider_car_bumped_time == 60) {
			rider_car_bumped = false;
			rider_car_bumped_time = 0;
		}
	}
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, -rider_car_cur[3][3][1]));
	/*ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_rider_car();*/

	rider_car_model_matrix[0] = ModelMatrix;

	draw_queue.push({ rider_car_cur[3][3][1], {0, 'r'} });

	
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-win_width / 16.0f * 5.0f, win_height / 16.0f * 7.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-win_width / 8.0f * (float(rider_car_bumped_num) / (BUMPED_MAX * 1.0f)), 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(win_width / (8.0f * STAMINAR_BAR_WIDTH) * float(BUMPED_MAX - rider_car_bumped_num) / BUMPED_MAX, win_height / (8.0f * STAMINAR_BAR_WIDTH), 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_staminar_bar();

	if (BUMPED_MAX <= rider_car_bumped_num) {
		glutLeaveMainLoop();
	}

	/*for (int i = 0; i < CAR_NUM; i++) {
		if (car_cur[i][3][3][1] < rider_car_cur[3][3][1]) {
			ModelMatrix = car_transform_matrix[i];
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car();
		}
	}*/


	// traverse the queue to draw the cars
	while (!draw_queue.empty()) {
		char c = draw_queue.top().second.second;
		switch (c){
			case 'c':
				ModelMatrix = car_model_matrix[draw_queue.top().second.first];
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_car(draw_queue.top().second.first);
				break;
			case 'r':
				ModelMatrix = rider_car_model_matrix[draw_queue.top().second.first];
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_rider_car();
				break;
			case 'p':
				ModelMatrix = person_model_matrix[draw_queue.top().second.first];
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_person(draw_queue.top().second.first);
				break;
			case 'h':
				ModelMatrix = house_model_matrix[draw_queue.top().second.first];
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_house(draw_queue.top().second.first);
				break;
			case 'f':
				ModelMatrix = falling_weapon_model_matrix[draw_queue.top().second.first];
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_airplane();
				break;
        }
		
        draw_queue.pop();
    }

	glFlush();	
}   

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

void special(int key, int x, int y) {
#define SENSITIVITY 2.0
	switch (key) {
	case GLUT_KEY_LEFT:
		centerx = ((rider_car_cur[0][3][0] - SENSITIVITY) > road_cur[0][3][0]) ? centerx - SENSITIVITY : centerx;
		// centerx -= SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		centerx = ((rider_car_cur[3][3][0] + SENSITIVITY) < road_cur[3][3][0]) ? centerx + SENSITIVITY : centerx;
		// centerx += SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		centery -= SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		centery += SENSITIVITY;
		glutPostRedisplay();
		break;
	}
}

int leftbuttonpressed = 0;
void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		leftbuttonpressed = 1;
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		leftbuttonpressed = 0;
}

void motion(int x, int y) {
} 
	
void reshape(int width, int height) {
	centerx = centerx * float(width) / float(win_width);
	win_width = width, win_height = height;
	
  	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0, 
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glClearColor(32.0f / 255.0f, 183.0f / 255.0f, 121.0f / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	initialize_timestamp(100);
	prepare_axes();
	prepare_line();
	prepare_airplane();
	prepare_cake();
	prepare_car();
	prepare_road();
	prepare_house();
	prepare_person();
	prepare_sky();
	prepare_shirt();
	prepare_ground();
	prepare_road_center_line();
	prepare_rider_car();
	prepare_fire();
	prepare_green_peak();
	prepare_staminar_bar();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program(); 
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

    error = glewInit();
	if (error != GLEW_OK) { 
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 2
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple2DTransformation_GLSL_3.0";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC', four arrows",
		"    - Mouse used: L-click and move"
	};

	glutInit (&argc, argv);
 	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize (1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop ();
}


