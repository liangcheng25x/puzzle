#include "SPAData.h"
#include "SenseModel.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace YAML;

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
    YAML::Node config = LoadFile( argv[1] );
    SenseModel senseModel( config["sensePara"] );
    SenseData senseData;
    
    Mat img;
    namedWindow("img", WINDOW_AUTOSIZE);
    mouse_info info = {0, Point(0, 0), Point(0, 0), Point(0, 0)};
    bool run = 0;
    
    while(1)
    {
        senseModel.run();
        senseModel.getImgs(&senseData);
        senseData.zed_rgb[0].copyTo(img);
        
        setMouseCallback("img", on_mouse, &info);

        //draw mouse information
        line(img, Point(info.ps.x, 0), Point(info.ps.x, img.rows), Scalar(0, 0, 255));
        line(img, Point(0, info.ps.y), Point(img.cols, info.ps.y), Scalar(0, 0, 255));
        rectangle(img, info.roi_tl, info.roi_br, Scalar(255, 0, 0));

        imshow("img", img);

        int kb = waitKey(10);
        if(kb == 27)
            break;
        if(kb ==32)
        {
            run = 1;
            break;
        }
    }
    
    destroyWindow("img");
    
    if(run)
    {
        ofstream fout( "work_space.yaml" );
        config["planPara"]["work space"]["x"] = info.roi_tl.x;
        config["planPara"]["work space"]["y"] = info.roi_tl.y;
        config["planPara"]["work space"]["w"] = info.roi_br.x - info.roi_tl.x;
        config["planPara"]["work space"]["h"] = info.roi_br.y - info.roi_tl.y;
        fout << config;
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
