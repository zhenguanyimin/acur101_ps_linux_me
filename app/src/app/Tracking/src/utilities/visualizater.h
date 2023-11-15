
#ifndef VISUALIZATION_H_
#define VISUALIZATION_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string>

#include "opencv2/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "common_struct.h"
#include "myMath.h"

using namespace std;
using namespace cv;


extern cv::Mat gVis_img;
extern double max_x;
extern double max_y;
extern double min_x;
extern double min_y;

cv::Mat get_mat();
void draw_point(cv::Mat &img, double x, double y, int size, cv::Scalar color, int flag);
void draw_text(cv::Mat &img, std::string text, double x, double y, double scale, \
	cv::Scalar color, int offset_x = 8, int offset_y = 8);
void draw_pointTxt(cv::Mat &img, double x, double y, int size, cv::Scalar color, \
	std::string text, int flag);
void draw_line(cv::Mat &img, double x0, double y0, double x1, double y1, cv::Scalar color, int thickness);
void draw_filledRectangle(cv::Mat &img, double x0, double y0, cv::Scalar color, int thickness);
void draw_rectangle(cv::Mat &img, double x0, double y0, double x1, double y1, cv::Scalar color, int thickness);
void draw_boundingbox(cv::Mat &img, double xCenter, double yCenter, double height, double width, cv::Scalar color, int thickness);
void draw_matrix(float *matrix, int col, int row);
cv::Mat catenate_images(cv::Mat img_left, cv::Mat img_right, int flag);

#endif
