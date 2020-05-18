#include <iostream>
#include "SPAData.h"
#include "SenseModel.h"
#include "PlanModel.h"
#include "HiwinSDK.h"
#define OPENCV
#include <yolo_v2_class.hpp>
#include "img_process.h"
#include "rgb_cf.h"

using namespace std;
using namespace cv;

void PlanModel::state_init(SenseData* senseData, PlanData* planData, ActData* actData)
{
    fragments.clear();
    
    //arm: go home
    Action init;
    init.arm.sw = 1;
    init.arm.coordType = HiwinSDK::CoordType::Joint;
    init.arm.moveType = HiwinSDK::MoveType::Absolute;
    init.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    init.arm.feedRate = 10;
    init.arm.value = {0, 0, 0, 0, -90, 0};

    //sucker
    init.sucker.resize(1);
    init.sucker[0].sw = 0;

    planData->action.push_back(init);

    //state switch
    switchMaster(MasterState::IDENTIFY);
}

void PlanModel::state_identify(SenseData* senseData, PlanData* planData, ActData* actData)
{
    //find puzzle in work space
    Mat ws_rgb(senseData->rs_rgb[0], roi);
    Mat ws_xyz(senseData->rs_xyz[0], roi);
    Mat ws_gray;
    cvtColor(ws_rgb, ws_gray, COLOR_BGR2GRAY);
    threshold(ws_gray, ws_gray, thres, THRESH_BINARY_INV);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( ws_gray, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE );

    //record and_img center xyz initial_angle
    for(size_t i = 0; i < contours.size(); i++)
    {
        RotatedRect roRect( minAreaRect(contours[i]) );
        Rect boundRect( roRect.boundingRect() );
        double x = senseData->ws_xyz[0].at<Vec3f>(boundRect.tl.y, boundRect.br.x) - senseData->ws_xyz[0].at<Vec3f>(boundRect.tl.y, boundRect.tl.x);
        double y = senseData->ws_xyz[0].at<Vec3f>(boundRect.br.y, boundRect.tl.x) - senseData->ws_xyz[0].at<Vec3f>(boundRect.tl.y, boundRect.tl.x);
        double area = x * y;
        if(900 < area && area < 3500)//unit: mm
        {
            fragment puzzle;

            //normalize img
            Mat and_img, rotate_img;
            bitwise_and(Mat(ws_rgb, boundRect), Mat(ws_rgb, boundRect), and_img, Mat(ws_gray, boundRect));
            rotateImg(and_img, rotate_img, roRect.angle, Scalar(0, 0, 0));

            puzzle.img = rotate_img.clone();
            puzzle.angle = -roRect.angle;
            puzzle.center[0] = roRect.center.x;
            puzzle.center[1] = roRect.center.y;
            puzzle.xyz[0] = ws_xyz.at<Vec3f>(roRect.center.y, roRect.center.x)[0];
            puzzle.xyz[1] = ws_xyz.at<Vec3f>(roRect.center.y, roRect.center.x)[1];
            puzzle.xyz[2] = ws_xyz.at<Vec3f>(roRect.center.y, roRect.center.x)[2];
            puzzle.state = 0;

            fragments.push_back(puzzle);
        }
    }
    
    //histogram
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

    //angle
    for(size_t i = 0; i < fragments.size(); i++)
    {
        vector<Mat> rotate_imgs(4);
        vector<double> diffirence;

        //resize
        Mat resized_sample, resized_fragment;
        if(sample[fragments[i].cls].img.total() <= fragments[i].img.total())
        {
            resize(fragments[i].img, resized_fragment, Size(sample[fragments[i].cls].img.cols, sample[fragments[i].cls].img.rows);
            resized_sample = sample[fragments[i].cls].img;
        }
        else
        {
            resize(sample[fragments[i].cls].img, resized_sample, Size(fragments[i].img.cols, fragments[i].img.rows);
            resized_fragment = fragments[i].cls;
        }

        for(size_t j = 0; j < rotate_imgs.size(); j++)
        {
            rotateImg(resized_fragment, rotate_imgs[j], j * 90.0, Scalar(0, 0, 0));
        }

        for(size_t j = 0; j < rotate_imgs.size(); j++)
        {
            Mat diffirence_img;
            absdiff(resized_sample, rotate_imgs[j], diffirence_img);
            Scalar s = sum(diffirence_img);
            cout << s << endl;
            diffirence.push_back();
        }
        
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
        
        fragments[i].angle += min_index;
        
//         if(fragments[i].angle > 180)
//             fragments[i].angle -= 360;
    }

    //corner point
    
    //arm
    Action identify;
    identify.arm.sw = 0;

    //sucker
    identify.sucker.resize(1);
    identify.sucker[0].sw = 0;

    planData->action.clear();
    planData->action.push_back(identify);

    //state switch
    switchMaster(MasterState::PIECE);
}

void PlanModel::state_piece(SenseData* senseData, PlanData* planData, ActData* actData)
{
    switch(slave)
    {
        case TRACE:
            cout << "piece trace" << endl;

            piece_trace(senseData, planData, actData);
            break;
            
        case CATCH:
            cout << "piece catch" << endl;
            
            piece_catch(senseData, planData, actData);
            break;
            
        case GO_PUT:
            cout << "piece go put" << endl;
            
            piece_go_put(senseData, planData, actData);
            break;

        case PUT:
            cout << "piece put" << endl;
            
            piece_put(senseData, planData, actData);
            break;
            
        case FINISH:
            cout << "piece finish" << endl;

            piece_finish(senseData, planData, actData);
            break;
    }
}

void PlanModel::state_finish(SenseData* senseData, PlanData* planData, ActData* actData)
{
    finish = 1;

    //arm
    Action go_home;
    go_home.arm.sw = 1;
    go_home.arm.coordType = HiwinSDK::CoordType::Joint;
    go_home.arm.moveType = HiwinSDK::MoveType::Absolute;
    go_home.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    go_home.arm.feedRate = 10;
    go_home.arm.value = {0, 0, 0, 0, -90, 0};

    //sucker
    go_home.sucker[0].sw = 0;

    planData->action.clear();
    planData->action.push_back(go_home);
}
