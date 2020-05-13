#include <iostream>
#include <opencv2/opencv.hpp>
#include "fm_clustering.h"
#include "graph_segmentation.h"
#define YAML_CPP
#include "realsense.h"

using namespace std;
using namespace cv;

typedef struct
{
    bool click;
    Point ps;
    Point roi_tl;
    Point roi_br;
}mouse_info;

void on_mouse(int event, int x, int y, int flags, void* para);

int main(int argc, char** argv)
{
    //camera setting
    RealSense rs( atoi(argv[1]), 1280, 720 );
    rs.set_align_direction(RealSense::AlignDirection(2));
    rs.set_hole_switch(1);
    rs.enable_rgb_auto_exposure(1);
    rs.set_decimation_value(1);

    //image setting
    Mat img;
    namedWindow("rs", WINDOW_AUTOSIZE);

    mouse_info info = {0, Point(0, 0), Point(0, 0), Point(0, 0)};
    bool run = 0;
    
    //catch image
    while(1)
    {
        rs.update();
        img = rs.get_rgb_image();

        setMouseCallback("rs", on_mouse, &info);

        //draw mouse information
        line(img, Point(info.ps.x, 0), Point(info.ps.x, img.rows), Scalar(0, 0, 255));
        line(img, Point(0, info.ps.y), Point(img.cols, info.ps.y), Scalar(0, 0, 255));
        rectangle(img, info.roi_tl, info.roi_br, Scalar(255, 0, 0));
        
        imshow("rs", img);

        int kb = waitKey(10);
        if(kb == 27)
            break;
        if(kb ==32)
        {
            run = 1;
            break;
        }
    }
    
    destroyWindow("rs");
    
    /*img = rs.get_rgb_image(Rect(info.roi_tl, info.roi_br));
    imshow("img", img);
    waitKey();*/
    
    if(run)
    {
        rs.update();
        img = rs.get_depth_mat(Rect(info.roi_tl, info.roi_br));

        /*FM_Clustering fc;
        fc.load_data(img);
        fc.k_means(3);*/

        Graph_Segmentation gs;
        gs.load_data(img);
        gs.process(0.5, atof(argv[2]), 50);
        Mat dst;
        gs.class2image(dst);

        imshow("dst", dst);
        waitKey();
        destroyWindow("dst");
    }
    
    return 0;
}

void on_mouse(int event, int x, int y, int flags, void* para)
{
    mouse_info* info = (mouse_info*)para;
    info->ps = Point(x, y);

    if(event == EVENT_LBUTTONDOWN && info->click == 0)
    {
        info->click = 1;
        info->roi_tl = info->ps;
        info->roi_br = info->ps;
    }
    else if(event == EVENT_LBUTTONUP && info->click == 1)
    {
        info->click = 0;
        info->roi_br = info->ps;
    }
}
