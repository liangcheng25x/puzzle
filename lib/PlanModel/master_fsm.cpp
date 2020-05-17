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

void PlanModel::state_init(SenseData* senseData, PlanData* planData)
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
    init.sucker.resize(suckerNum);
    for(int i = 0; i < suckerNum; i++)
        init.sucker[i].sw = 0;

    planData->action.push_back(init);

    //state switch
    switchMaster(MasterState::IDENTIFY);
}

void PlanModel::state_identify(SenseData* senseData, PlanData* planData)
{
    //find puzzle in work space
    Mat ws_rgb(senseData->rs_rgb[0], roi);
    Mat ws_xyz(senseData->rs_xyz[0], roi);
    Mat gray;
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
    
    //act
    planData->action.arm.sw = 0;
    planData->action.gripper.sw = 0;
    planData->action.sucker.sw = 0;

    while(waitKey(10) != 32) {}

    //state switch
    switchMaster(MasterState::PIECE);
}

void PlanModel::state_piece(SenseData* senseData, PlanData* planData)
{

    //act
    planData->action.arm.sw = 0;
    planData->action.gripper.sw = 0;
    planData->action.sucker.sw = 0;

    //state switch
    switchMaster(MasterState::BOARD_EXIST_CHECK);
}


void PlanModel::ai_play(SenseData* senseData, PlanData* planData)
{
    switch(slave)
    {
        case AI_INIT:
            cout << "ai init" << endl;
            
            ai_init(senseData, planData);
            break;
            
        case POSITION_DECIDE:
            cout << "ai decide position" << endl;
            
            position_decide(senseData, planData);
            break;
            
        case SKY_MOVE_TO_CUBE:
            cout << "ai sky move to cube" << endl;
            
            ai_sky_move_to_cube(senseData, planData);
            break;

        case ARM_MOVE_TO_CUBE:
            cout << "ai move to cube" << endl;
            
            ai_arm_move_to_cube(senseData, planData);
            break;
            
        case CATCH_CUBE:
            cout << "ai catch cube" << endl;
            
            ai_catch_cube(senseData, planData);
            break;

        case FINISH_CATCH_CUBE:
            cout << "ai finish catch cube" << endl;
            
            ai_finish_catch_cube(senseData, planData);
            break;
            
        case MOVE_TO_BOARD:
            cout << "ai move to board" << endl;
            
            ai_move_to_board(senseData, planData);
            break;
            
        case PUT_CUBE:
            cout << "ai put cube" << endl;
            
            ai_put_cube(senseData, planData);
            break;
            
        case AI_FINISH:
            cout << "ai finish" << endl;
            
            ai_finish(senseData, planData);
            break;
    }
}

void PlanModel::state_finish(SenseData* senseData, PlanData* planData)
{
    finish = 1;

    //move
    planData->action.arm.sw = 1;
    planData->action.arm.coordType = Hiwin::CoordType::Joint;
    planData->action.arm.moveType = Hiwin::MoveType::Absolute;
    planData->action.arm.ctrlType = Hiwin::CtrlType::Linear;
    planData->action.arm.feedRate = 10;
    planData->action.arm.value = {0, 0, 0, 0, -90, 0};
    planData->action.gripper.sw = 0;
    planData->action.sucker.sw = 0;
}
