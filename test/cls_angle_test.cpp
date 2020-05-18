#include <iostream>
#include <opencv2/opencv.hpp>
#include "img_process.h"
#include "rgb_cf.h"

using namespace std;
using namespace cv;

typedef struct
{
    cv::Mat img;
    std::array<int, 2> center;
    std::array<double, 3> xyz;
    int cls;
    double angle;
}fragment;

typedef struct
{
    cv::Mat img;
    int cls;
    double xyz;
    bool linked;
}specimen;

int main(int argc, char** argv)
{
    Mat img = imread(argv[1]);
    Rect roi(Point(120, 0), Point(1120, 720));
    
    Mat ws_rgb(img, roi);
    Mat ws_gray;
    cvtColor(ws_rgb, ws_gray, COLOR_BGR2GRAY);
    int thres = atoi(argv[2]);
    threshold(ws_gray, ws_gray, thres, 255, THRESH_BINARY_INV);
    
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( ws_gray, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE );

    vector<fragment> fragments;
    
    for(size_t i = 0; i < contours.size(); i++)
    {
        RotatedRect roRect( minAreaRect(contours[i]) );
        Rect boundRect( roRect.boundingRect() );
        double area = contourArea(contours[i]);
        
        if(5000 < area && area < 30000)//unit: mm
        {
            fragment puzzle;

            Scalar color( 0, 255, 0 );
            drawContours( ws_rgb, contours, i, color, 2, 8);

            //normalize img
            Mat and_img, rotate_img;
            bitwise_and(Mat(ws_rgb, boundRect), Mat(ws_rgb, boundRect), and_img, Mat(ws_gray, boundRect));
            rotateImage(and_img, rotate_img, roRect.angle, Scalar(0, 0, 0));

            puzzle.img = rotate_img.clone();
            puzzle.angle = -roRect.angle;
            puzzle.center[0] = roRect.center.x;
            puzzle.center[1] = roRect.center.y;
//             puzzle.xyz[0] = ws_xyz.at<Vec3f>(roRect.center.y, roRect.center.x)[0];
//             puzzle.xyz[1] = ws_xyz.at<Vec3f>(roRect.center.y, roRect.center.x)[1];
//             puzzle.xyz[2] = ws_xyz.at<Vec3f>(roRect.center.y, roRect.center.x)[2];

            fragments.push_back(puzzle);
        }
    }
    
    cout << "fragment length: " << fragments.size() << endl;
    for(size_t i = 0; i < fragments.size(); i++)
    {
        imshow("img" + to_string(i), fragments[i].img);
    }
    
    imshow("rgb", ws_rgb);
    imshow("gray", ws_gray);
    waitKey();

    return 0;
}
