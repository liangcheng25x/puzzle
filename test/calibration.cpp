#include <iostream>
#include <yaml-cpp/yaml.h>
#include "SPAData.h"
#include "SenseModel.h"
#include "PlanModel.h"
#include "yaml.h"
#include "img_process.h"

using namespace std;
using namespace cv;

static void on_trackbar( int, void* )
{
    
}

static void min_on_trackbar( int min, void* n) {
    double* minArea = (double*) n;
    *minArea = min * 100;
}

static void max_on_trackbar( int max, void* x) {
    double* maxArea = (double*) x;
    *maxArea = max * 100;
}

int main(int argc, char** argv)
{
    YAML::Node node = YAML::LoadFile( argv[1] );

    SenseModel senseModel;
    senseModel.init(node["sense"]);

    SenseData senseData;
    PlanData planData;
    ActData actData;
    
    Rect roi;
    roi = node["plan"]["workspace"].as<Rect>();
    int thres = 0;
    int min = 0;
    double minArea;
    int max = 0;
    double maxArea;
    
    namedWindow("rs", WINDOW_AUTOSIZE);
    namedWindow("control", WINDOW_AUTOSIZE);
    namedWindow("thresh", WINDOW_AUTOSIZE);
    createTrackbar( "thre", "control", &thres, 255, on_trackbar );
    createTrackbar( "min", "control", &min, 30, min_on_trackbar, &minArea );
    createTrackbar( "max", "control", &max, 100, max_on_trackbar, &maxArea );

    while(true)
    {
        senseModel.run();
        senseModel.getImgs(&senseData);
        
        cameraCoord2armCoord(senseData.rs_xyz[0], senseData.rs_xyz[0], 1000);

        Mat ws_rgb(senseData.rs_rgb[0], roi);
        Mat ws_xyz(senseData.rs_xyz[0], roi);
        Mat ws_depth(senseData.rs_depth[0], roi);

        //set threshold for depth image
        Mat ws_gray_depth, ws_thresh_depth;
        cvtColor(ws_depth, ws_gray_depth, COLOR_BGR2GRAY);
        threshold(ws_gray_depth, ws_thresh_depth, thres, 255, THRESH_BINARY);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours( ws_thresh_depth, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE );

        //record and_img center xyz initial_angle
        for(size_t i = 0; i < contours.size(); i++)
        {
            RotatedRect roRect( minAreaRect(contours[i]) );
            Rect boundRect( roRect.boundingRect() );
            double x = ws_xyz.at<Vec3f>(boundRect.tl().y, boundRect.br().x)[0] - ws_xyz.at<Vec3f>(boundRect.tl().y, boundRect.tl().x)[0];
            double y = ws_xyz.at<Vec3f>(boundRect.tl().y, boundRect.tl().x)[1] - ws_xyz.at<Vec3f>(boundRect.br().y, boundRect.tl().x)[1];
            double area = abs(x) * abs(y);
            if(minArea < area && area < maxArea)//unit: mm
            {
                Point2f vertices[4];
                roRect.points(vertices);
                vector<vector<Point>> roVertices;
                vector<Point> roVertice;
                for(int i = 0; i < 4; i++)
                {
                    roVertice.push_back(vertices[i]);
                }
                roVertices.push_back(roVertice);
                drawContours( senseData.rs_depth[0], roVertices, 0, 255, 2, 8, noArray(), INT_MAX, roi.tl());
            }
        }

        imshow("rs", senseData.rs_depth[0]);
        imshow("thresh", ws_thresh_depth);
        char c = waitKey(10);
        if(c == 27) break;
    }

    return 0;
}
