#include <iostream>
#include <opencv2\opencv.hpp>

cv::Mat problem_a_rotate_forward(cv::Mat img, double angle) {
	cv::Mat output;
	//////////////////////////////////////////////////////////////////////////////
	//                         START OF YOUR CODE                               //
	//////////////////////////////////////////////////////////////////////////////
	///// hint : refer annotation /////
	// 
	//Calculate Rotation matrix
	//
	//Find (x,y)ranges of output
	//
	//Calculate 'output' size  from range of output
	//
	//Calculate destination coordinates from origin
	//

	typedef struct Point {
		int x, y;
	} Point;

	Point points[4];

	double radian = (double)(-angle * CV_PI / 180);
	double sin_radian = (double)(sin(radian));
	double cos_radian = (double)(cos(radian));

	int img_width = (int)img.cols;
	int img_height = (int)img.rows;

	points[0] = { 0, 0 };
	points[1] = { (int)(round(img_height * cos_radian)), (int)(round(img_height * sin_radian)) };
	points[2] = { (int)(round(points[1].x - img_width * sin_radian)), (int)(round(points[1].y + img_width * cos_radian)) };
	points[3] = { (int)(round(points[0].x - img_width * sin_radian)), (int)(round(img_width * cos_radian)) };

	int max_x = std::max(std::max(points[0].x, points[1].x), std::max(points[2].x, points[3].x));
	int min_x = std::min(std::min(points[0].x, points[1].x), std::min(points[2].x, points[3].x));

	int max_y = std::max(std::max(points[0].y, points[1].y), std::max(points[2].y, points[3].y));
	int min_y = std::min(std::min(points[0].y, points[1].y), std::min(points[2].y, points[3].y));

	output.create((max_x - min_x), (max_y - min_y), img.type());
	output.setTo(0);

	int output_width = (int)output.cols;
	int output_height = (int)output.rows;

	Point img_center = { (int)round(img_height / 2), (int)round(img_width / 2) };
	Point output_center = { (int)round(output_height / 2), (int)round(output_width / 2) };


	for (int x = 0; x < img_height; x++) {
		for (int y = 0; y < img_width; y++) {
			int temp_x = x - img_center.x;
			int temp_y = y - img_center.y;
			double rotated_x = (temp_x * cos_radian) - (temp_y * sin_radian);
			double rotated_y = (temp_x * sin_radian) + (temp_y * cos_radian);
			int moved_x = (int)round(rotated_x + output_center.x);
			int moved_y = (int)round(rotated_y + output_center.y);

			if (!(0 <= moved_x && moved_x < output_height && 0 <= moved_y && moved_y < output_width)) {
				continue;
			}
			for (int color = 0; color < img.channels(); color++) {
				output.at<cv::Vec3b>(moved_x, moved_y)[color] = img.at<cv::Vec3b>(x, y)[color];
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//                          END OF YOUR CODE                                //
	//////////////////////////////////////////////////////////////////////////////


	cv::imshow("a_output", output); cv::waitKey(0);
	return output;
}

cv::Mat problem_b_rotate_backward(cv::Mat img, double angle) {
	cv::Mat output;

	//////////////////////////////////////////////////////////////////////////////
	//                         START OF YOUR CODE                               //
	//////////////////////////////////////////////////////////////////////////////
	///// hint : refer annotation /////
	// 
	//Calculate Rotation matrix
	// 
	//Calculate Inverse Rotation matrix
	//
	//Find (x,y)ranges of output
	//
	//Calculate 'output' size  from range of output
	//
	//Calculate origin coordinates from destination coordinates
	//

	typedef struct Point {
		int x, y;
	} Point;

	Point points[4];

	double radian = (double)(-angle * CV_PI / 180);
	double sin_radian = (double)(sin(radian));
	double cos_radian = (double)(cos(radian));

	int img_width = (int)img.cols;
	int img_height = (int)img.rows;


	cv::Mat rotation_matrix;
	rotation_matrix.create(2, 2, CV_32F);
	float* rotation_mat = (float*)rotation_matrix.data;
	rotation_mat[0] = (float)cos_radian;
	rotation_mat[1] = -(float)sin_radian;
	rotation_mat[2] = (float)sin_radian;
	rotation_mat[3] = (float)cos_radian;


	cv::Mat inverse_matrix = rotation_matrix.inv();

	points[0] = { 0, 0 };
	points[1] = { (int)(round(img_height * cos_radian)), (int)(round(img_height * sin_radian)) };
	points[2] = { (int)(round(points[1].x - img_width * sin_radian)), (int)(round(points[1].y + img_width * cos_radian)) };
	points[3] = { (int)(round(points[0].x - img_width * sin_radian)), (int)(round(img_width * cos_radian)) };

	int max_x = std::max(std::max(points[0].x, points[1].x), std::max(points[2].x, points[3].x));
	int min_x = std::min(std::min(points[0].x, points[1].x), std::min(points[2].x, points[3].x));

	int max_y = std::max(std::max(points[0].y, points[1].y), std::max(points[2].y, points[3].y));
	int min_y = std::min(std::min(points[0].y, points[1].y), std::min(points[2].y, points[3].y));

	output.create((max_x - min_x), (max_y - min_y), img.type());
	output.setTo(0);

	int output_width = (int)output.cols;
	int output_height = (int)output.rows;

	Point img_center = { (int)round(img_height / 2), (int)round(img_width / 2) };
	Point output_center = { (int)round(output_height / 2), (int)round(output_width / 2) };

	for (int x = 0; x < output_height; x++) {
		for (int y = 0; y < output_width; y++) {
			int temp_x = x - output_center.x;
			int temp_y = y - output_center.y;

			cv::Mat origin_matrix;
			origin_matrix.create(2, 1, CV_32F);
			origin_matrix.setTo(0);
			float* origin_mat = (float*)origin_matrix.data;

			cv::Mat rotated_matrix;
			rotated_matrix.create(2, 1, CV_32F);

			float* rotated_mat = (float*)rotated_matrix.data;
			rotated_mat[0] = (float)temp_x;
			rotated_mat[1] = (float)temp_y;

			origin_matrix = inverse_matrix * rotated_matrix;

			int origin_x = (int)(origin_mat[0] + img_center.x);
			int origin_y = (int)(origin_mat[1] + img_center.y);

			if (!(0 <= origin_x && origin_x < img_height && 0 <= origin_y && origin_y < img_width)) {
				continue;
			}
			for (int color = 0; color < img.channels(); color++) {
				output.at<cv::Vec3b>(x, y)[color] = img.at<cv::Vec3b>(origin_x, origin_y)[color];
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//                          END OF YOUR CODE                                //
	//////////////////////////////////////////////////////////////////////////////

	cv::imshow("b_output", output); cv::waitKey(0);

	return output;
}

cv::Mat problem_c_rotate_backward_interarea(cv::Mat img, double angle) {
	cv::Mat output;

	//////////////////////////////////////////////////////////////////////////////
	//                         START OF YOUR CODE                               //
	//////////////////////////////////////////////////////////////////////////////
	///// hint : refer annotation /////
	// 
	//Calculate Rotation matrix
	// 
	//Calculate Inverse Rotation matrix
	//
	//Find (x,y)ranges of output
	//
	//Calculate 'output' size  from range of output
	//
	//Calculate origin coordinates from destination coordinates
	//
	////////INTER_AREA
	//


	typedef struct Point {
		int x, y;
	} Point;

	Point points[4];

	double radian = (double)(-angle * CV_PI / 180);
	double sin_radian = (double)(sin(radian));
	double cos_radian = (double)(cos(radian));

	int img_width = (int)img.cols;
	int img_height = (int)img.rows;


	cv::Mat rotation_matrix;
	rotation_matrix.create(2, 2, CV_32F);
	float* rotation_mat = (float*)rotation_matrix.data;
	rotation_mat[0] = (float)cos_radian;
	rotation_mat[1] = -(float)sin_radian;
	rotation_mat[2] = (float)sin_radian;
	rotation_mat[3] = (float)cos_radian;

	cv::Mat inverse_matrix = rotation_matrix.inv();

	points[0] = { 0, 0 };
	points[1] = { (int)(round(img_height * cos_radian)), (int)(round(img_height * sin_radian)) };
	points[2] = { (int)(round(points[1].x - img_width * sin_radian)), (int)(round(points[1].y + img_width * cos_radian)) };
	points[3] = { (int)(round(points[0].x - img_width * sin_radian)), (int)(round(img_width * cos_radian)) };

	int max_x = std::max(std::max(points[0].x, points[1].x), std::max(points[2].x, points[3].x));
	int min_x = std::min(std::min(points[0].x, points[1].x), std::min(points[2].x, points[3].x));

	int max_y = std::max(std::max(points[0].y, points[1].y), std::max(points[2].y, points[3].y));
	int min_y = std::min(std::min(points[0].y, points[1].y), std::min(points[2].y, points[3].y));

	output.create((max_x - min_x), (max_y - min_y), img.type());
	output.setTo(0);

	int output_width = (int)output.cols;
	int output_height = (int)output.rows;

	for (int x = 0; x < output_height; x++) {
		for (int y = 0; y < output_width; y++) {
			float temp_x = (float)(x - (float)output_height / 2);
			float temp_y = (float)(y - (float)output_width / 2);

			cv::Mat origin_matrix;
			origin_matrix.create(2, 1, CV_32F);
			origin_matrix.setTo(0);
			float* origin_mat = (float*)origin_matrix.data;

			cv::Mat rotated_matrix;
			rotated_matrix.create(2, 1, CV_32F);

			float* rotated_mat = (float*)rotated_matrix.data;
			rotated_mat[0] = (float)temp_x;
			rotated_mat[1] = (float)temp_y;

			origin_matrix = inverse_matrix * rotated_matrix;

			float origin_x = origin_mat[0] + (float)img_height / 2;
			float origin_y = origin_mat[1] + (float)img_width / 2;

			int origin_x1 = (int)floor(origin_x);
			int origin_y1 = (int)floor(origin_y);

			int origin_x2 = (int)ceil(origin_x);
			int origin_y2 = (int)ceil(origin_y);


			if (!(0 <= origin_x1 && origin_x2 < img_height && 0 <= origin_y1 && origin_y2 < img_width)) {
				continue;
			}

			// Interpolation
			for (int color = 0; color < img.channels(); color++) {
				double A = img.at<cv::Vec3b>(origin_x2, origin_y1)[color];
				double B = img.at<cv::Vec3b>(origin_x1, origin_y1)[color];
				double C = img.at<cv::Vec3b>(origin_x1, origin_y2)[color];
				double D = img.at<cv::Vec3b>(origin_x2, origin_y2)[color];

				double h1 = (double)((float)origin_x2 - origin_x);
				double h2 = 1 - h1;

				double M = B * h1 + A * h2;
				double N = C * h1 + D * h2;

				double w1 = (double)(origin_y - (float)origin_y1);
				double w2 = 1 - w1;

				double P = M * w2 + N * w1;

				output.at<cv::Vec3b>(x, y)[color] = (uchar)P;
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////////
	//                          END OF YOUR CODE                                //
	//////////////////////////////////////////////////////////////////////////////


	cv::imshow("c_output", output); cv::waitKey(0);

	return output;
}

int main(void) {

	double angle = -15.0f;

	cv::Mat input = cv::imread("lena.jpg");
	//Fill problem_a_rotate_forward and show output
	problem_a_rotate_forward(input, angle);
	//Fill problem_b_rotate_backward and show output
	problem_b_rotate_backward(input, angle);
	//Fill problem_c_rotate_backward_interarea and show output
	problem_c_rotate_backward_interarea(input, angle);
}