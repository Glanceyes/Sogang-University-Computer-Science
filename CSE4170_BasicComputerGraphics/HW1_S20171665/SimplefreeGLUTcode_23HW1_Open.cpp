#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define TO_RADIAN		0.017453292519943296 
#define PI			3.14159265358979323846

int rightbuttonpressed = 0;
int leftbuttonpressed = 0, center_selected = 0;

float r, g, b; // Background color
float px, py, qx, qy; // Line (px, py) --> (qx, qy)
int n_object_points = 6;
float object[6][2], object_center_x, object_center_y;
float rotation_angle_in_degree;
int window_width, window_height;

float rectangle[4][2], rectangle_center_x, rectangle_center_y;

float mouse_cur_x, mouse_cur_y, mouse_prev_x, mouse_prev_y;
float mouse_dx, mouse_dy;
bool p_pressed_by_left_button;

void matrix_mul(float (*a)[3], float (*b)[1]) {
	/*
		a: 3 X 3 matrix
		b: 3 X 1 matrix
	*/
	float temp[3][3] = { 0 };

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 1; j++) {
			for (int k = 0; k < 3; k++) {
				temp[i][j] += a[i][k] * b[k][j];
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 1; j++) {
			b[i][j] = temp[i][j];
		}
	}
}

void translate_line(float dx, float dy, bool include_q) {
	float trans_param[3][3] = { 0 };

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == j) {
				trans_param[i][j] = 1;
			}
		}
	}

	trans_param[0][2] = dx;
	trans_param[1][2] = dy;


	float vertex[3][1] = { 0 };
	vertex[0][0] = px;
	vertex[1][0] = py;
	vertex[2][0] = 1;

	matrix_mul(trans_param, vertex);

	px = vertex[0][0];
	py = vertex[1][0];

	if (include_q) {
		vertex[0][0] = qx;
		vertex[1][0] = qy;
		vertex[2][0] = 1;

		matrix_mul(trans_param, vertex);

		qx = vertex[0][0];
		qy = vertex[1][0];
	}
}

void rotate_line() {
	float dx, dy;

	dx = px;
	dy = py;

	translate_line(-dx, -dy, true);

	float trans_param[3][3] = { 0 };

	trans_param[0][0] = cos(rotation_angle_in_degree);
	trans_param[0][1] = -sin(rotation_angle_in_degree);
	trans_param[1][0] = sin(rotation_angle_in_degree);
	trans_param[1][1] = cos(rotation_angle_in_degree);
	trans_param[2][2] = 1;

	float vertex[3][1] = { 0 };

	vertex[0][0] = qx;
	vertex[1][0] = qy;
	vertex[2][0] = 1;

	matrix_mul(trans_param, vertex);

	qx = vertex[0][0];
	qy = vertex[1][0];

	translate_line(dx, dy, true);
}

void translate_polygon(float dx, float dy, float (*object)[2], float *object_center_x, float *object_center_y, int vertex_num) {
	float trans_param[3][3] = { 0 };

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == j) {
				trans_param[i][j] = 1;
			}
		}
	}

	trans_param[0][2] = dx;
	trans_param[1][2] = dy;

	for (int i = 0; i < vertex_num; i++) {
		float vertex[3][1] = { 0 };
		vertex[0][0] = object[i][0];
		vertex[1][0] = object[i][1];
		vertex[2][0] = 1;

		matrix_mul(trans_param, vertex);

		object[i][0] = vertex[0][0];
		object[i][1] = vertex[1][0];
	}

	float vertex[3][1] = { 0 };
	vertex[0][0] = *object_center_x;
	vertex[1][0] = *object_center_y;
	vertex[2][0] = 1;

	matrix_mul(trans_param, vertex);

	*object_center_x = vertex[0][0];
	*object_center_y = vertex[1][0];
}

void scale_polygon(float scaling_factor, float (*object)[2], float *object_center_x, float *object_center_y, int vertex_num) {
	float trans_param[3][3] = { 0 };
	float dx, dy;

	dx = *object_center_x;
	dy = *object_center_y;

	translate_polygon(-dx, -dy, object, object_center_x, object_center_y, vertex_num);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == j) {
				if (i < 2) {
					trans_param[i][j] = scaling_factor;
				}
				else {
					trans_param[i][j] = 1;
				}
			}
		}
	}

	for (int i = 0; i < vertex_num; i++) {
		float vertex[3][1] = { 0 };
		vertex[0][0] = object[i][0];
		vertex[1][0] = object[i][1];
		vertex[2][0] = 1;

		matrix_mul(trans_param, vertex);

		object[i][0] = vertex[0][0];
		object[i][1] = vertex[1][0];
	}

	translate_polygon(dx, dy, object, object_center_x, object_center_y, vertex_num);
}

void rotate_polygon(float rotation_angle, float(*object)[2], float* object_center_x, float* object_center_y, int vertex_num) {
	float trans_param[3][3] = { 0 };
	float dx, dy;

	dx = *object_center_x;
	dy = *object_center_y;

	translate_polygon(-dx, -dy, object, object_center_x, object_center_y, vertex_num);

	trans_param[0][0] = cos(rotation_angle);
	trans_param[0][1] = -sin(rotation_angle);
	trans_param[1][0] = sin(rotation_angle);
	trans_param[1][1] = cos(rotation_angle);
	trans_param[2][2] = 1;

	for (int i = 0; i < vertex_num; i++) {
		float vertex[3][1] = { 0 };
		vertex[0][0] = object[i][0];
		vertex[1][0] = object[i][1];
		vertex[2][0] = 1;

		matrix_mul(trans_param, vertex);

		object[i][0] = vertex[0][0];
		object[i][1] = vertex[1][0];
	}

	translate_polygon(dx, dy, object, object_center_x, object_center_y, vertex_num);
}

void shear_polygon(bool x_axis, float shearing_factor, float(*object)[2], float *object_center_x, float *object_center_y, int vertex_num){ 
	float trans_param[3][3] = { 0 };
	float dx, dy;

	dx = *object_center_x;
	dy = *object_center_y;

	translate_polygon(-dx, -dy, object, object_center_x, object_center_y, vertex_num);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == j) {
				trans_param[i][j] = 1;
			}
		}
	}
	
	if (x_axis) {
		trans_param[0][1] = shearing_factor;
	}
	else {
		trans_param[1][0] = shearing_factor;
	}

	for (int i = 0; i < vertex_num; i++) {
		float vertex[3][1] = { 0 };
		vertex[0][0] = object[i][0];
		vertex[1][0] = object[i][1];
		vertex[2][0] = 1;

		matrix_mul(trans_param, vertex);

		object[i][0] = vertex[0][0];
		object[i][1] = vertex[1][0];
	}

	translate_polygon(dx, dy, object, object_center_x, object_center_y, vertex_num);
}

void draw_axes() {
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.975f, 0.025f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.975f, -0.025f);
	glVertex2f(1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.025f, 0.975f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(-0.025f, 0.975f);
	glVertex2f(0.0f, 1.0f);
	glEnd();
	glLineWidth(1.0f);

	glPointSize(7.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glEnd();
	glPointSize(1.0f);
}
void draw_line(float px, float py, float qx, float qy) {
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(px, py); 
	glVertex2f(qx, qy);
	glEnd();
	glPointSize(7.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(px, py);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(qx, qy);
	glEnd();
	glPointSize(1.0f);
}

void draw_object(void) {
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 1.0f, 0.0f);
	for (int i = 0; i < 6; i++)
		glVertex2f(object[i][0], object[i][1]);
	glEnd();
	glPointSize(7.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 6; i++)
		glVertex2f(object[i][0], object[i][1]);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(object_center_x, object_center_y);
	glEnd();
	glPointSize(1.0f);
}

void draw_rectangle(void) {
	glLineWidth(3.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 4; i++) {
		if (i < 2) {
			glColor3f(0.0f, 0.5f, 1.0f);
			glVertex2f(rectangle[i][0], rectangle[i][1]);
		}
		else {
			glColor3f(0.5f, 0.5f, 0.0f);
			glVertex2f(rectangle[i][0], rectangle[i][1]);
		}
	}
	glEnd();
	glLineWidth(1.0f);
	glPointSize(8.0f);
	glBegin(GL_POINTS);
	glColor3f(0.8f, 0.8f, 0.8f);
	for (int i = 0; i < 4; i++) {
		glVertex2f(rectangle[i][0], rectangle[i][1]);
	}
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(rectangle_center_x, rectangle_center_y);
	glEnd();
	glPointSize(1.0f);
}

void display(void) {
	glClearColor(r, g, b, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT);

	draw_axes();
	draw_line(px, py, qx, qy);
	draw_object();
	draw_rectangle();
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'r':
		r = 1.0f; g = b = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'g':
		g = 1.0f; r = b = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'b':
		b = 1.0f; r = g = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 's':
		r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'q':
		glutLeaveMainLoop(); 
		break;
	}

}

void special(int key, int x, int y) {
	int mod = glutGetModifiers();
	switch (key) {
	case GLUT_KEY_LEFT:
		if (mod == GLUT_ACTIVE_CTRL) {
			shear_polygon(true, -0.1f, rectangle, &rectangle_center_x, &rectangle_center_y, 4);
		}
		else if (mod == GLUT_ACTIVE_ALT) {
			rotate_polygon(TO_RADIAN, rectangle, &rectangle_center_x, &rectangle_center_y, 4);
		}
		else {
			r -= 0.1f;
			if (r < 0.0f) r = 0.0f;
			fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		}
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		if (mod == GLUT_ACTIVE_CTRL) {
			shear_polygon(true, 0.1f, rectangle, &rectangle_center_x, &rectangle_center_y, 4);
		}
		else if (mod == GLUT_ACTIVE_ALT) {
			rotate_polygon(-TO_RADIAN, rectangle, &rectangle_center_x, &rectangle_center_y, 4);
		}
		else {
			r += 0.1f;
			if (r > 1.0f) r = 1.0f;
			fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		}
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		if (mod == GLUT_ACTIVE_CTRL) {
			shear_polygon(false, -0.1f, rectangle, &rectangle_center_x, &rectangle_center_y, 4);
		}
		else {
			g -= 0.1f;
			if (g < 0.0f) g = 0.0f;
			fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		}
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		if (mod == GLUT_ACTIVE_CTRL) {
			shear_polygon(false, 0.1f, rectangle, &rectangle_center_x, &rectangle_center_y, 4);
		}
		else {
			g += 0.1f;
			if (g > 1.0f) g = 1.0f;
			fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		}
		glutPostRedisplay();
		break;
	}
}

int prevx, prevy;
void mousepress(int button, int state, int x, int y) {
	mouse_cur_x = ((float)x - ((float)window_width / 2)) / 250.0f;
	mouse_cur_y = -(((float)y - ((float)window_height / 2)) / 250.0f);

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			leftbuttonpressed = 1;
			if (mouse_cur_x >= px - (5.f / 250.f) && mouse_cur_x <= px + (5.f / 250.f)
				&& mouse_cur_y >= py - (5.f / 250.f) && mouse_cur_y <= py + (5.f / 250.f)) {
				p_pressed_by_left_button = true;
			}
		}
		else if (state == GLUT_UP) {
			leftbuttonpressed = 0;
			p_pressed_by_left_button = false;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			rightbuttonpressed = 1;
		}
		else if (state == GLUT_UP) {
			rightbuttonpressed = 0;
		}
	}
	else if (button == 3) {
		rotation_angle_in_degree = TO_RADIAN;
		rotate_line();
		glutPostRedisplay();
	}
	else if (button == 4) {
//		PI / 180.0 * TO_RADIAN * 10
		rotation_angle_in_degree = -TO_RADIAN;
		rotate_line();
		glutPostRedisplay();
	}

	mouse_prev_x = mouse_cur_x;
	mouse_prev_y = mouse_cur_y;
}

void mousemove(int x, int y) {
	int mod = glutGetModifiers();

	mouse_cur_x = ((float)x - ((float)window_width / 2)) / 250.0f;
	mouse_cur_y = -(((float)y - ((float)window_height / 2)) / 250.0f);
	
	mouse_dx = mouse_cur_x - mouse_prev_x;
	mouse_dy = mouse_cur_y - mouse_prev_y;

//	fprintf(stdout, "$$$ The mouse moved from (%lf, %lf) to (%lf, %lf).\n", mouse_prev_x, mouse_prev_y, mouse_cur_x, mouse_cur_y);

	if (mod == GLUT_ACTIVE_CTRL) {
		if (rightbuttonpressed == 1) {
			if (mouse_dx < 0) {
				scale_polygon((float)0.95, object, &object_center_x, &object_center_y, 6);
			}
			else if (mouse_dx > 0) {
				scale_polygon((float)1.05, object, &object_center_x, &object_center_y, 6);
			}
			glutPostRedisplay();
		}
		else if (leftbuttonpressed == 1) {
			if (mouse_dx < 0) {
				scale_polygon((float)0.95, rectangle, &rectangle_center_x, &rectangle_center_y, 4);
			}
			else if (mouse_dx > 0) {
				scale_polygon((float)1.05, rectangle, &rectangle_center_x, &rectangle_center_y, 4);
			}
			glutPostRedisplay();
		}
	}
	else if (mod == GLUT_ACTIVE_ALT) {
		if (rightbuttonpressed == 1) {
			translate_polygon(mouse_dx, mouse_dy, object, &object_center_x, &object_center_y, 6);
			glutPostRedisplay();
		}
		else if (leftbuttonpressed == 1) {
			translate_polygon(mouse_dx, mouse_dy, rectangle, &rectangle_center_x, &rectangle_center_y, 4);
			glutPostRedisplay();
		}
	}
	else if (mod == GLUT_ACTIVE_SHIFT) {
		if (leftbuttonpressed == 1) {
			if (p_pressed_by_left_button) {
				mouse_dx = mouse_cur_x - px;
				mouse_dy = mouse_cur_y - py;
				translate_line(mouse_dx, mouse_dy, false);
			}

			/*
			if ((mouse_cur_x >= px - 0.01f && mouse_cur_x <= px + 0.01f 
				&& mouse_cur_y >= py - 0.01f && mouse_cur_y <= py + 0.01f)) {
				translate_line(mouse_dx, mouse_dy, false);
			}
			*/

			glutPostRedisplay();
		}
		else {
//			p_pressed_by_left_button = false;
		}
	}
	mouse_prev_x = mouse_cur_x;
	mouse_prev_y = mouse_cur_y;
}

void mousepassivemove(int x, int y) {
//	p_pressed_by_left_button = false;
	mouse_cur_x = ((float)x - ((float)window_width / 2)) / 250.0f;
	mouse_cur_y = -(((float)y - ((float)window_height / 2)) / 250.0f);

	mouse_prev_x = mouse_cur_x;
	mouse_prev_y = mouse_cur_y;
}
	
void reshape(int width, int height) {
	// DO NOT MODIFY THIS FUNCTION!!!
	window_width = width, window_height = height;
	glViewport(0.0f, 0.0f, window_width, window_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-window_width / 500.0f, window_width / 500.0f,  -window_height / 500.0f, window_height / 500.0f, -1.0f, 1.0f);

	glutPostRedisplay();
}


void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemove);
//	glutPassiveMotionFunc(mousepassivemove);
	glutReshapeFunc(reshape);
 	glutCloseFunc(close);
}

void initialize_renderer(void) {
	register_callbacks();
	r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f; // Background color = Salmon
	px = -0.70f, py = 0.35f, qx = -0.20f, qy = -0.10f;
	rotation_angle_in_degree = 1.0f; // 1 degree
	
	float sq_cx = 0.35f, sq_cy = -0.25f, sq_side = -0.35f;
	object[0][0] = sq_cx + 1.25 * sq_side;
	object[0][1] = sq_cy + 1.5 * sq_side;
	object[1][0] = sq_cx + 0.05 * sq_side;
	object[1][1] = sq_cy + 2 * sq_side;
	object[2][0] = sq_cx - 1.25 * sq_side;
	object[2][1] = sq_cy + sq_side;
	object[3][0] = sq_cx - sq_side;
	object[3][1] = sq_cy - 0.5 * sq_side;
	object[4][0] = sq_cx - 0.15 * sq_side;
	object[4][1] = sq_cy - 0.75 * sq_side;
	object[5][0] = sq_cx + sq_side;
	object[5][1] = sq_cy - 0.35 * sq_side;
	object_center_x = object_center_y = 0.0f;
	for (int i = 0; i < n_object_points; i++) {
		object_center_x += object[i][0];
		object_center_y += object[i][1];
	}
	object_center_x /= n_object_points;
	object_center_y /= n_object_points;

	float rt_cx = -0.25f, rt_cy = 0.25f, rt_side = 0.4f;
	rectangle[0][0] = rt_cx + 1.0f * rt_side;
	rectangle[0][1] = rt_cy - 1.0f * rt_side;
	rectangle[1][0] = rt_cx - 1.0f * rt_side;
	rectangle[1][1] = rt_cy - 1.0f * rt_side;
	rectangle[2][0] = rt_cx - 1.0f * rt_side;
	rectangle[2][1] = rt_cy + 1.0f * rt_side;
	rectangle[3][0] = rt_cx + 1.0f * rt_side;
	rectangle[3][1] = rt_cy + 1.0f * rt_side;

	rectangle_center_x = rectangle_center_y = 0.0f;
	for (int i = 0; i < 4; i++) {
		rectangle_center_x += rectangle[i][0];
		rectangle_center_y += rectangle[i][1];
	}
	rectangle_center_x /= 4;
	rectangle_center_y /= 4;
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
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

#define N_MESSAGE_LINES 4
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple 2D Transformations";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'r', 'g', 'b', 's', 'q'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: SHIFT/L-click and move, ALT/R-click and move, CTRL/R-click and move",
		"    - Wheel used: up and down scroll"
		"    - Other operations: window size change"
	};

	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); // <-- Be sure to use this profile for this example code!
//	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(750, 750);
	glutInitWindowPosition(500, 200);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

//	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT); // default
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	
	glutMainLoop();
	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");
}