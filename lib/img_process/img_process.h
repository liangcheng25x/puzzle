#ifndef IMG_PROCESS_H
#define IMG_PROCESS_H

#include <opencv2/opencv.hpp>

void rotateImageInboundRect(cv::Mat& img, cv::Mat& dst, double degree, cv::Scalar color);
void cameraCoord2armCoord(cv::Mat& src, cv::Mat& dst, double m);
void relaCoord2absoCoord(cv::Mat& src, cv::Mat& dst, std::vector<double> base);

#endif
