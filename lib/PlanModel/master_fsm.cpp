#include <iostream>
#include "PlanModel.h"
#include "img_process.h"
#include "rgb_cf.h"

using namespace std;
using namespace cv;

void PlanModel::state_init(SenseData* senseData, PlanData* planData, ActData* actData)
{
    //arm: go home
    Action init;
    init.arm.sw = 1;
    init.arm.coordType = HiwinSDK::CoordType::Coord;
    init.arm.moveType = HiwinSDK::MoveType::Absolute;
    init.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    init.arm.feedRate = 10;
    init.arm.value = work_posture;

    //sucker
    init.sucker.resize(1);
    init.sucker[0].sw = 0;

    planData->action.push_back(init);

    //state switch
    switchMaster(MasterState::IDENTIFY);
}

void PlanModel::state_identify(SenseData* senseData, PlanData* planData, ActData* actData)
{
    //coordinate transform
    cameraCoord2armCoord(senseData->rs_xyz[0], senseData->rs_xyz[0], 1000);
    relaCoord2absoCoord(senseData->rs_xyz[0], senseData->rs_xyz, actData->currentPos);
    
    //extract interesting region of mat
    Mat ws_rgb(senseData->rs_rgb[0], roi);
    Mat ws_xyz(senseData->rs_xyz[0], roi);
    Mat ws_depth(senseData->rs_depth[0], roi);

    //set threshold for depth image
    Mat ws_gray_depth, ws_thresh_depth;
    cvtColor(ws_depth, ws_gray_depth, COLOR_BGR2GRAY);
    threshold(ws_gray_depth, ws_thresh_depth, thres, THRESH_BINARY);

    //find all contours
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( ws_thresh_depth, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE );

    //record and_img center xyz initial_angle
    for(size_t i = 0; i < contours.size(); i++)
    {
        RotatedRect roRect( minAreaRect(contours[i]) );
        Rect boundRect( roRect.boundingRect() );
        double x = senseData->ws_xyz[0].at<Vec3f>(boundRect.tl.y, boundRect.br.x) - senseData->ws_xyz[0].at<Vec3f>(boundRect.tl.y, boundRect.tl.x);
        double y = senseData->ws_xyz[0].at<Vec3f>(boundRect.tl.y, boundRect.tl.x) - senseData->ws_xyz[0].at<Vec3f>(boundRect.br.y, boundRect.tl.x);
        double area = abs(x) * abs(y);
        if(900 < area && area < 3500)//unit: mm
        {
            fragment puzzle;
            
            //produce mask
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

            //enter information
            rotate_img.copyTo(puzzle.img);
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
    Surf_CF surf_cf;
    surf_cf.init(samples);
    for(size_t i = 0; i < fragments.size(); i++)
    {
        vector<similarity_list> list;
        list = surf_cf.classify(fragments[i].img);
        if(list[1].similarity > 2)
            fragments[i].cls = list[0].cls;
        else
            fragments[i].cls = list[1].cls;
    }

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
    
    //act
    planData->action.clear();

    //state switch
    switchMaster(MasterState::PIECE);
}

void PlanModel::state_piece(SenseData* senseData, PlanData* planData, ActData* actData)
{
    switch(slave)
    {
        case DECISION:
            cout << "piece decision" << endl;

            piece_decision(senseData, planData, actData);
            break;

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
