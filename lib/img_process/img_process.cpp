#include "img_process.h"

void rotateImage(Mat& img, Mat& dst, double degree, Scalar color)
{
    float angle = degree * CV_PI / 180;
    double a = sin(angle), b = cos(angle);
    int rotate_width = img.rows * fabs(a) + img.cols * fabs(b);
    int rotate_height = img.cols * fabs(a) + img.rows * fabs(b);

    Point2f center = Point2f(img.cols / 2., img.rows / 2.);
    Mat rot_mat = getRotationMatrix2D(center, degree, 1.0);

    rot_mat.at<double>(0, 2) += (rotate_width - img.cols) / 2;
    rot_mat.at<double>(1, 2) += (rotate_height - img.rows) / 2;

    Size img_size(rotate_width, rotate_height);

    warpAffine(img, dst, rot_mat, img_size, INTER_NEAREST, BORDER_CONSTANT, color));
}
