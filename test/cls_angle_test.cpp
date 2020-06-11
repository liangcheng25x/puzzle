#include <iostream>
#include <opencv2/opencv.hpp>
#include "img_process.h"
//#include "rgb_cf.h"
#include "cf.h"
#include <yaml-cpp/yaml.h>
#include "yaml.h"

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
    YAML::Node node = YAML::LoadFile( argv[1] );
    YAML::Node config = node["planPara"];
    Mat rgb = imread(argv[2]);
    Mat depth = imread(argv[3]);
    Rect roi;
    roi = Rect(Point( config["work space"]["x"].as<int>(), config["work space"]["y"].as<int>() ), Size( config["work space"]["w"].as<int>(), config["work space"]["h"].as<int>() ));

    Mat ws_depth(depth, roi);
    Mat ws_rgb(rgb, roi);
    Mat ws_gray;
    //Mat mask = Mat::zeros(ws_depth.size(), CV_8U);
    cvtColor(ws_depth, ws_gray, COLOR_BGR2GRAY);
    int thres = config["threshold"].as<int>();
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

//             Scalar color( 0, 255, 0 );
//             drawContours( ws_rgb, contours, i, color, 2, 8);

            //normalize img
            Mat and_img, rotate_img;
            Mat mask = Mat::zeros(ws_depth.size(), CV_8U);
            Point2f vertices[4];
            roRect.points(vertices);
            vector<vector<Point>> roVertices;
            vector<Point> roVertice;
            for(int i = 0; i < 4; i++)
            {
                roVertice.push_back(vertices[i]);
            }
            roVertices.push_back(roVertice);
            drawContours( mask, roVertices, 0, 255, -1);
            bitwise_and(Mat(ws_rgb, boundRect), Mat(ws_rgb, boundRect), and_img, Mat(mask, boundRect));
            rotateImageInboundRect(and_img, rotate_img, roRect.angle, Scalar(0, 0, 0));

            rotate_img.copyTo(puzzle.img);
            puzzle.angle = -roRect.angle;
            puzzle.center[0] = roRect.center.x;
            puzzle.center[1] = roRect.center.y;
//             puzzle.xyz[0] = ws_xyz.at<Vec3f>(roRect.center.y, roRect.center.x)[0];
//             puzzle.xyz[1] = ws_xyz.at<Vec3f>(roRect.center.y, roRect.center.x)[1];
//             puzzle.xyz[2] = ws_xyz.at<Vec3f>(roRect.center.y, roRect.center.x)[2];

            fragments.push_back(puzzle);
            
//             imshow("rotate img", rotate_img);
//             imshow("and img", and_img);
//             imshow("mask", mask);
//             waitKey();
        }
    }

    vector<Mat> samples;
    for(size_t i = 0; i < config["sample"].size(); i++)
    {
        Mat tmp = imread(config["sample"][i].as<string>());
        samples.push_back(tmp);
    }

    /*RGB_CF rgb_cf;
    rgb_cf.init(samples);
    for(size_t i = 0; i < fragments.size(); i++)
    {
        vector<similarity_list> list;
        list = rgb_cf.classify(fragments[i].img);
        fragments[i].cls = list[0].cls;
    }*/
    
    cout << "classify" << endl;
    
    Surf_CF surf_cf;
    surf_cf.init(samples);
    cout << "surf init" << endl;
    for(size_t i = 0; i < fragments.size(); i++)
    {
        vector<cls_info> list;
        list = surf_cf.classify(fragments[i].img, 5);
        cout << "0: " << list[0].sl << ", cls: " << list[0].cls << endl;
        cout << "1: " << list[1].sl << ", cls: " << list[1].cls << endl;
        cout << "2: " << list[2].sl << ", cls: " << list[2].cls << endl;
        if(list[0].sl > 30)
            fragments[i].cls = list[0].cls;
        else
            fragments[i].cls = list[list.size()-1].cls;
    }
    
    vector<specimen> sample(samples.size());
    for(size_t i = 0; i < sample.size(); i++)
        sample[i].img = samples[i];

    cout << "angle" << endl;
    
    //angle
    for(size_t i = 0; i < fragments.size(); i++)
    {
        //produce rotate fragments
        vector<Mat> rotate_imgs(4);

        for(size_t j = 0; j < rotate_imgs.size(); j++)
        {
            rotateImage(fragments[i].img, rotate_imgs[j], j * -90.0, Scalar(0, 0, 0));
        }

        //resize samples and unknown fragments
        Mat resized_sample;
        vector<double> diffirence(4);

        for(size_t j = 0; j < rotate_imgs.size(); j++)
        {
            if( sample[fragments[i].cls].img.total() <= rotate_imgs[j].total() )
            {
                resize(rotate_imgs[j], rotate_imgs[j], Size(sample[fragments[i].cls].img.cols, sample[fragments[i].cls].img.rows));
                resized_sample = sample[fragments[i].cls].img;
            }
            else
            {
                resize(sample[fragments[i].cls].img, resized_sample, Size(rotate_imgs[j].cols, rotate_imgs[j].rows));
            }

            //count diffirence between sample and rotated fragments
            Mat diff_img;
            absdiff(resized_sample, rotate_imgs[j], diff_img);
            Scalar s = sum(diff_img);
            //cout << s[0] + s[1] + s[2] << endl;
            diffirence[j] = s[0] + s[1] + s[2];
        }

        //find minimun diffirence to decide angle
        int min_index = 0;
        double min = diffirence[0];
        for(size_t j = 0; j < diffirence.size(); j++)
        {
            if(diffirence[j] < min)
            {
                min_index = j;
                min = diffirence[j];
            }
        }

        fragments[i].angle += min_index * 90;

        if(fragments[i].angle > 180)
            fragments[i].angle -= 360;
    }
    
    for(size_t i = 0; i < fragments.size(); i++)
    {
        //imshow("img" + to_string(i), fragments[i].img);
        cout << "img " << i << " class: " << fragments[i].cls << endl;
        cout << "center: " << fragments[i].center[0] << ", " << fragments[i].center[1] << endl;
        cout << "angle: " << fragments[i].angle << endl;
        putText(ws_rgb, to_string(fragments[i].cls) + ", " + to_string((int)(fragments[i].angle)), Point(fragments[i].center[0], fragments[i].center[1]), 0, 0.8, Scalar(255, 255, 255), 2);
        circle(ws_rgb, Point(fragments[i].center[0], fragments[i].center[1]), 3, Scalar(0, 0, 255), -1, 8);
//         waitKey();
//         destroyWindow("img" + to_string(i));
    }

    imshow("rgb", ws_rgb);
    imshow("gray", ws_gray);
    waitKey();

    return 0;
}
