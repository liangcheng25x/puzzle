#include "img_process.h"

using namespace cv;

void rotateImageInboundRect(Mat& img, Mat& dst, double degree, Scalar color)
{
    if(degree == -45)
        degree = -44.9;
    float angle = degree * CV_PI / 180;
    double a = sin(-angle), b = cos(-angle);
//     int rotate_width = img.rows * fabs(a) + img.cols * fabs(b);
//     int rotate_height = img.cols * fabs(a) + img.rows * fabs(b);

//     double cos_sin = pow(b, 2) - pow(a, 2);
//     if(cos_sin == 0)
//         cos_sin = 2;
    int rotate_width = (img.cols * b - img.rows * a) / (pow(b, 2) - pow(a, 2));
    int rotate_height = (img.rows * b - img.cols * a) / (pow(b, 2) - pow(a, 2));

    Point2f center = Point2f(img.cols / 2., img.rows / 2.);
    Mat rot_mat = getRotationMatrix2D(center, degree, 1.0);

    rot_mat.at<double>(0, 2) += (int)((rotate_width - img.cols) / 2);
    rot_mat.at<double>(1, 2) += (int)((rotate_height - img.rows) / 2);

    Size img_size(rotate_width, rotate_height);

    warpAffine(img, dst, rot_mat, img_size, INTER_NEAREST, BORDER_CONSTANT, color);
}

void cameraCoord2armCoord(cv::Mat& src, cv::Mat& dst, double m)
{
    Mat _dst(src.size(), src.type());
    
    for(int i = 0; i < _dst.rows; i++)
    {
        for(int j = 0; j < _dst.cols; j++)
        {
            _dst.at<Vec3f>(i, j)[0] = m * src.at<Vec3f>(i, j)[0];
            _dst.at<Vec3f>(i, j)[1] = -1 * m * src.at<Vec3f>(i, j)[1];
            _dst.at<Vec3f>(i, j)[2] = -1 * m * src.at<Vec3f>(i, j)[2];
        }
    }
    
    _dst.copyTo(dst);
}

void relaCoord2absoCoord(cv::Mat& src, cv::Mat& dst, std::vector<double> base)
{
    Mat _dst(src.size(), src.type());

    for(int i = 0; i < _dst.rows; i++)
    {
        for(int j = 0; j < _dst.cols; j++)
        {
            _dst.at<Vec3f>(i, j)[0] += base[0];
            _dst.at<Vec3f>(i, j)[1] += base[1];
            _dst.at<Vec3f>(i, j)[2] += base[2];
        }
    }

    _dst.copyTo(dst);
}
