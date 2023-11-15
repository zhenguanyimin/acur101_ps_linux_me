
#include "common_struct.h"

#ifdef VISUALIZATION_GTRACK
#include "visualizater.h"

static cv::Mat gVis_img; // = cv::Mat::zeros(cv::Size(IMG_WIDTH, IMG_HEIGHT), CV_8UC3);

double max_x = 1200;
double max_y = -600;
double min_x = 0;
double min_y = 0;

#define color_snow cv::Scalar(250, 250, 255)
#define color_red cv::Scalar(0, 0, 255)
#define color_green cv::Scalar(0, 255, 0)
#define color_blue cv::Scalar(250, 0, 0)
#define color_grey41 cv::Scalar(105, 105, 105)
#define color_grey31 cv::Scalar(79, 79, 79)
#define color_grey71 cv::Scalar(207, 207, 207)

cv::Mat get_mat()
{
	int x_array[10] = {int(IMG_HEIGHT*0.1), int(IMG_HEIGHT*0.2), int(IMG_HEIGHT*0.3), \
		int(IMG_HEIGHT*0.4), int(IMG_HEIGHT*0.5), int(IMG_HEIGHT*0.6), \
		int(IMG_HEIGHT*0.7), int(IMG_HEIGHT*0.8), int(IMG_HEIGHT*0.9), int(IMG_HEIGHT*1.1) };
	int y_array[10] = {int(IMG_WIDTH/10)*1, int(IMG_WIDTH/10)*2, int(IMG_WIDTH/10)*3, \
		int(IMG_WIDTH/10)*4, int(IMG_WIDTH/10)*5, int(IMG_WIDTH / 10) * 6, int(IMG_WIDTH / 10) * 7, \
		int(IMG_WIDTH/10)*8, int(IMG_WIDTH/10)*9, int(IMG_WIDTH/10)*10};
	int x0 = int(IMG_HEIGHT/HEIGHT_SPLIT_SCALE*(HEIGHT_SPLIT_SCALE-1));
	int y0 = int(IMG_WIDTH*0.5);
	gVis_img = cv::Mat::zeros(cv::Size(IMG_WIDTH, IMG_HEIGHT), CV_8UC3);

	max_x = inv_convert_x(0);
	min_x = inv_convert_x(IMG_HEIGHT);
	max_y = inv_convert_y(0);
	min_y = inv_convert_y(IMG_WIDTH);

	for (int i = 0; i < 10; i++) {
		//cv::line(gVis_img, cv::Point(1, x_array[i]), \
		//	cv::Point(IMG_WIDTH-1, x_array[i]), color_grey31);
		double y_ = inv_convert_y(y_array[i]);
		draw_text(gVis_img, std::to_string(static_cast<long long>(y_)), \
			inv_convert_x(x0), y_, 1, color_grey71, -20, 0);
	}
	for (int i = 0; i < 10; i++) {
		//cv::line(gVis_img, cv::Point(x_array[i], 0), \
		//	cv::Point(x_array[i], IMG_HEIGHT-1), color_grey31);
		double x_ = inv_convert_x(x_array[i]);
		draw_text(gVis_img, std::to_string(static_cast<long long>(x_)), \
			x_, min_y, 1, color_grey71, 0, 35);
	}
	cv::line(gVis_img, cv::Point(y0, 0), cv::Point(y0, IMG_HEIGHT-1), cv::Scalar(125,125,125));
	cv::line(gVis_img, cv::Point(0, x0), cv::Point(IMG_WIDTH-1, x0), cv::Scalar(125,125,125));
	// 45 deg
	//cv::line(gVis_img, cv::Point(y0, x0), cv::Point(IMG_WIDTH - IMG_HEIGHT/ HEIGHT_SPLIT_SCALE,0), cv::Scalar(125, 125, 125));
	//cv::line(gVis_img, cv::Point(y0, x0), cv::Point(IMG_HEIGHT / HEIGHT_SPLIT_SCALE,0), cv::Scalar(125, 125, 125));
	// 60deg
	cv::line(gVis_img, cv::Point(y0, x0), cv::Point(IMG_WIDTH - 1,IMG_HEIGHT - IMG_HEIGHT / HEIGHT_SPLIT_SCALE - (int)(IMG_WIDTH / 2.f*tanf(30.f*DEG2RAD))), cv::Scalar(125, 125, 125));
	cv::line(gVis_img, cv::Point(y0, x0), cv::Point(0, IMG_HEIGHT - IMG_HEIGHT / HEIGHT_SPLIT_SCALE - (int)(IMG_WIDTH / 2.f*tanf(30.f*DEG2RAD))), cv::Scalar(125, 125, 125));
	// 30deg
	cv::line(gVis_img, cv::Point(y0, x0), cv::Point(IMG_WIDTH / 2 + (int)((double)(IMG_HEIGHT - IMG_HEIGHT / HEIGHT_SPLIT_SCALE)*tanf(30.f*DEG2RAD)), 0), cv::Scalar(125, 125, 125));
	cv::line(gVis_img, cv::Point(y0, x0), cv::Point(IMG_WIDTH / 2 - (int)((double)(IMG_HEIGHT - IMG_HEIGHT / HEIGHT_SPLIT_SCALE)*tanf(30.f*DEG2RAD)), 0), cv::Scalar(125, 125, 125));


	draw_text(gVis_img, "x/m", inv_convert_x(30), 0, 2, color_grey71, 10, 0);
	draw_text(gVis_img, "y/m", 10, inv_convert_y(20), 2, color_grey71, 0, 0);
	draw_point(gVis_img, 0.f, 0.f, 200, cv::Scalar(125, 125, 125), 1.0);
	draw_point(gVis_img, 0.f, 0.f, 500, cv::Scalar(125, 125, 125), 1.0);
	draw_point(gVis_img, 0.f, 0.f, 1000, cv::Scalar(125, 125, 125), 1.0);

	// Draw lane
	/*cv::line(gVis_img, cv::Point(convert_y(3.75/2.0), 0), \
		cv::Point(convert_y(3.75/2.0), IMG_HEIGHT-1), cv::Scalar(50,100,200));
	cv::line(gVis_img, cv::Point(convert_y(3.75/2.0+3.75), 0), \
		cv::Point(convert_y(3.75/2.0+3.75), IMG_HEIGHT-1), cv::Scalar(50,100,200));
	cv::line(gVis_img, cv::Point(convert_y(3.75/2.0+3.75+3.75), 0), \
		cv::Point(convert_y(3.75/2.0+3.75+3.75), IMG_HEIGHT-1), cv::Scalar(50,100,200));
	cv::line(gVis_img, cv::Point(convert_y(-3.75/2.0), 0), \
		cv::Point(convert_y(-3.75/2.0), IMG_HEIGHT-1), cv::Scalar(50,100,200));
	cv::line(gVis_img, cv::Point(convert_y(-3.75/2.0-3.75), 0), \
		cv::Point(convert_y(-3.75/2.0-3.75), IMG_HEIGHT-1), cv::Scalar(50,100,200));
	cv::line(gVis_img, cv::Point(convert_y(-3.75/2.0-3.75-3.75), 0), \
		cv::Point(convert_y(-3.75/2.0-3.75-3.75), IMG_HEIGHT-1), cv::Scalar(50,100,200));*/

	return gVis_img.clone();
}

void draw_point(cv::Mat &img, double x, double y, int size, cv::Scalar color, int flag)
{
	cv::Point p(convert_y(y), convert_x(x));
	cv::circle(img, p, (int)((double)(size)*IMG_HEIGHT_SCALE), color, flag);
}
void draw_text(cv::Mat &img, std::string text, double x, double y, double scale, \
	cv::Scalar color, int offset_x, int offset_y)
{
	int x0 = 0;
	int y0 = 0;
	x0 = convert_x(x);
	y0 = convert_y(y);
	cv::putText(img, text, cv::Point(y0 - offset_y, x0 - offset_x), \
		FONT_HERSHEY_PLAIN, scale, color, 1);
}

void draw_pointTxt(cv::Mat &img, double x, double y, int size, cv::Scalar color, \
					std::string text, int flag)
{
	int x0 = 0;
	int y0 = 0;
	x0 = convert_x(x);
	y0 = convert_y(y);
	cv::circle(img, cv::Point(y0,x0), size, color, flag);
	cv::putText(img, text, cv::Point(y0 - 8, x0 - 8), FONT_HERSHEY_PLAIN, 1, color, 1);
}

void draw_line(cv::Mat &img, double x0, double y0, double x1, double y1, cv::Scalar color, int thickness)
{
	cv::Point p0(convert_y(y0), convert_x(x0));
	cv::Point p1(convert_y(y1), convert_x(x1));
	cv::line(img, p0, p1, color, thickness);
}

void draw_filledRectangle(cv::Mat &img, double x0, double y0, cv::Scalar color, int thickness)
{
	int x0_ = 0;
	int y0_ = 0;
	y0_ = convert_y(y0);
	x0_ = convert_x(x0);
	cv::rectangle(img, cv::Point(y0_-3, x0_-3), cv::Point(y0_+3, x0_+3), color, thickness);
}

void draw_rectangle(cv::Mat &img, double x0, double y0, double x1, double y1, cv::Scalar color, int thickness)
{
	int x0_ = convert_x(x0);
	int y0_ = convert_y(y0);
	int x1_ = convert_x(x1);
	int y1_ = convert_y(y1);
	cv::rectangle(img, cv::Point(y0_, x0_), cv::Point(y1_, x1_), color, thickness);
}

void draw_boundingbox(cv::Mat &img, double xCenter, double yCenter, double height, double width, cv::Scalar color, int thickness)
{
	int x0_ = convert_x(xCenter);
	int y0_ = convert_y(yCenter);
	int height_pixel = (int)(height * IMG_HEIGHT_SCALE);
	int width_pixel = (int)(width * IMG_WIDTH_SCALE);
	int y_tl = y0_ - (int)(width_pixel/2);
	int x_tl = x0_ - (int)(height_pixel/2);
	int y_br = y0_ + (int)(width_pixel/2);
	int x_br = x0_ + (int)(height_pixel/2);
	float alpha = 0.7f;
	double max_color = 0;
	int max_idx = 0;
	for (int i = 0; i < 3; i++) {
		if (color.val[i] > max_color) {
			max_color = color.val[i];
			max_idx = i;
		}
	}
	int color_[3] = {0};
	color_[max_idx] = 255;
	cv::Scalar color_tmp;
	color_tmp.val[0] = color.val[0] * alpha + color_[0] * (1.0-alpha);
	color_tmp.val[1] = color.val[1] * alpha + color_[1] * (1.0-alpha);
	color_tmp.val[2] = color.val[2] * alpha + color_[2] * (1.0-alpha);
	cv::rectangle(img, cv::Point(y_tl, x_tl), cv::Point(y_br, x_br), color_tmp, thickness);
}

cv::Mat catenate_images(cv::Mat img_left, cv::Mat img_right, int flag)
{
	if(flag==1){
		int w1 = img_left.cols; int h1 = img_left.rows;
		int w2 = img_right.cols; int h2 = img_right.rows;
		int width = w1 + w2; int height = max(h1, h2);
		cv::Mat  resultImg = cv::Mat(height, width, CV_8UC3, cv::Scalar::all(0));
		cv::Mat ROI_1 = resultImg(cv::Rect(0, 0, w1, h1));
		cv::Mat ROI_2 = resultImg(cv::Rect(w1, 0, w2, h2));
		img_left.copyTo(ROI_1);
		img_right.copyTo(ROI_2);
		return resultImg.clone();
	}
	else if(flag==0){
		return img_right.clone();
	}
}

void draw_matrix(float *matrix, int col, int row)
{
//	cv::Mat img = cv::Mat::zeros(cv::Size(row, col), CV_8UC1);
//	float max = 0;
//	float min = 100000;
//	for (int i = 0; i < row; i++) {
//		for (int j = 0; j < col; j++) {
//			int idx = i * col + j;
//			if (matrix[idx] < min) {
//				min = matrix[idx];
//			}
//			if (matrix[idx] > max) {
//				max = matrix[idx];
//			}
//
//		}
//	}
//	for (int i = 0; i < row; i++) {
//		for (int j = 0; j < col; j++) {
//			int idx = i * col + j;
//			img.at<uchar>(j,i) = int(matrix[idx] / (max - min) * 255);
//		}
//	}
//	cv::imshow("matrix", img);
//	cv::waitKey(1);
}


#endif


