#ifndef IMG_PROCESS_H
#define IMG_PROCESS_H

#include <opencv2/opencv.hpp>

void rotateImage(cv::Mat& img, cv::Mat& dst, double degree, cv::Scalar color);

#endif
