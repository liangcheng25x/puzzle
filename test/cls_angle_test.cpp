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
    Mat rgb = imread(argv[1]);
    Mat depth = imread(argv[2]);
    Rect roi;
    roi = Rect(Point( config["work space"]["x"].as<int>(), config["work space"]["y"].as<int>() ), Size( config["work space"]["w"].as<int>(), config["work space"]["h"].as<int>() ));

    Mat ws_depth(depth, roi);
    Mat ws_rgb(rgb, roi);
    Mat ws_gray;
    cvtColor(ws_depth, ws_gray, COLOR_BGR2GRAY);
    int thres = atoi(argv[3]);
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

            /*Scalar color( 0, 255, 0 );
            drawContours( ws_rgb, contours, i, color, 2, 8);*/

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

    vector<Mat> samples(sample.size());
    for(size_t i = 0; i < sample.size(); i++)
    {
        samples[i] = sample[i].img;
    }
    RGB_CF rgb_cf;
    rgb_cf.init(samples);
    for(size_t i = 0; i < fragments.size(); i++)
    {
        vector<similarity_list> list;
        list = rgb_cf.classify(fragments[i].img);
        fragments[i].cls = list[0].cls;
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
