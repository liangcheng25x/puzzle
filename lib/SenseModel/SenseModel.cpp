#include <iostream>
#include "SenseModel.h"

using namespace std;
using namespace cv;
using namespace sl;

SenseModel::SenseModel()
{
    cout << "SenseModel()" << endl;
}

SenseModel::~SenseModel()
{
    cout << "~SenseModel()" << endl;

    for(int i = 0; i < zedNum; i++)
        if(zed[i].isOpened())
            zed[i].close();
}

void SenseModel::init(const YAML::Node& sense)
{
    //camera number
    if(sense["rs"])
        rsNum = sense["rs"].size();
    else
        rsNum = 0;

    if(sense["zed"])
        zedNum = sense["zed"].size();
    else
        zedNum = 0;

    //initialize Camera
    rs.resize(rsNum);
    for(int i = 0; i < rsNum; i++)
    {
        rs[i] = sense["rs"][i].as<RealSense>();
        rs[i].set_align_direction(RealSense::AlignDirection(2));
        rs[i].set_colorizer_type(RealSense::ColorizerType(3));
        rs[i].set_hole_switch(1);
    }

    zed = new Camera[zedNum];
    for(int i = 0; i < zedNum; i++)
    {
        zed[i].open(sense["zed"][i].as<InitParameters>());
        
        YAML::Node vedio_setting = sense["zed"][i]["vedio setting"];
        zed[i].setCameraSettings(sl::VIDEO_SETTINGS::BRIGHTNESS, vedio_setting["brightness"].as<int>());
        zed[i].setCameraSettings(sl::VIDEO_SETTINGS::CONTRAST, vedio_setting["contrast"].as<int>());
        zed[i].setCameraSettings(sl::VIDEO_SETTINGS::HUE, vedio_setting["hue"].as<int>());
        zed[i].setCameraSettings(sl::VIDEO_SETTINGS::GAMMA, vedio_setting["gamma"].as<int>());
        zed[i].setCameraSettings(sl::VIDEO_SETTINGS::GAIN, vedio_setting["gain"].as<int>());
        zed[i].setCameraSettings(sl::VIDEO_SETTINGS::EXPOSURE, vedio_setting["exposure"].as<int>());
        zed[i].setCameraSettings(sl::VIDEO_SETTINGS::WHITEBALANCE_TEMPERATURE, vedio_setting["whitebalance temperature"].as<int>());
        zed[i].setCameraSettings(sl::VIDEO_SETTINGS::WHITEBALANCE_AUTO, vedio_setting["whitebalance auto"].as<bool>());
        
    }

    rs_rgb.resize(rsNum);
    rs_v_depth.resize(rsNum);
    rs_real_mat.resize(rsNum);

    sl_rgb.resize(zedNum);
    sl_depth.resize(zedNum);
    sl_point_cloud.resize(zedNum);
    
    zed_rgb.resize(zedNum);
    zed_v_depth.resize(zedNum);
    zed_point_cloud.resize(zedNum);
}

void SenseModel::run()
{
    //real sense
    for(int i = 0; i < rsNum; i++)
    {
        rs[i].update();
        rs_rgb[i] = rs[i].get_rgb_image();
        rs_v_depth[i] = rs[i].get_depth_image();
        rs_real_mat[i] = rs[i].get_xyz();
    }

    //zed
    for(int i = 0; i < zedNum; i++)
    {
        zed[i].grab();

        zed[i].retrieveImage(sl_rgb[i], VIEW::LEFT);
        zed[i].retrieveImage(sl_depth[i], VIEW::DEPTH);
        zed[i].retrieveMeasure(sl_point_cloud[i], MEASURE::XYZ);

        slMat2cvMat(sl_rgb[i]).copyTo(zed_rgb[i]);
        slMat2cvMat(sl_depth[i]).copyTo(zed_v_depth[i]);
        slMat2cvMat(sl_point_cloud[i]).copyTo(zed_point_cloud[i]);

        /*zed_rgb[i] = slMat2cvMat(rgb);
        zed_v_depth[i] = slMat2cvMat(v_depth);
        zed_point_cloud[i] = slMat2cvMat(point_cloud);*/
    }
}

/*void SenseModel::drawImgs(PlanData planData)
{
    for(unsigned int i = 0; i < planData.drawRect.size(); i++)
    {
        Rect rect = planData.drawRect[i].bboxs;
        Scalar color = planData.drawRect[i].bboxColors;
        string msg = planData.drawRect[i].bboxMsgs;
        int camid = planData.drawRect[i].cam;
        
        rectangle(rgb[camid], rect, color, 2);
        putText(rgb[camid], msg, rect.tl(), 0, 1, color, 2);
        
        rectangle(v_depth[camid], rect, color, 2);
        putText(v_depth[camid], msg, rect.tl(), 0, 1, color, 2);
    }
}*/

void SenseModel::showImgs()
{
    for(int i = 0; i < rsNum; i++)
    {
        string rgb_name = "rs rgbImg" + to_string(i);
        namedWindow(rgb_name, WINDOW_AUTOSIZE);
        imshow(rgb_name, rs_rgb[i]);

        string depth_name = "rs depthImg" + to_string(i);
        namedWindow(depth_name, WINDOW_AUTOSIZE);
        imshow(depth_name, rs_v_depth[i]);
    }

    for(int i = 0; i < zedNum; i++)
    {
        string rgb_name = "zed rgbImg" + to_string(i);
        namedWindow(rgb_name, WINDOW_AUTOSIZE);
        imshow(rgb_name, zed_rgb[i]);

        string depth_name = "zed depthImg" + to_string(i);
        namedWindow(depth_name, WINDOW_AUTOSIZE);
        imshow(depth_name, zed_v_depth[i]);
    }
}

void SenseModel::getImgs(SenseData* senseData)
{
    senseData->rs_rgb.resize(rsNum);
    senseData->rs_depth.resize(rsNum);
    senseData->rs_xyz.resize(rsNum);
    
    senseData->zed_rgb.resize(zedNum);
    senseData->zed_depth.resize(zedNum);
    senseData->zed_xyz.resize(zedNum);

    for(int i = 0; i < rsNum; i++)
    {
        senseData->rs_rgb[i] = rs_rgb[i].clone();
        senseData->rs_depth[i] = rs_v_depth[i].clone();
        senseData->rs_xyz[i] = rs_real_mat[i].clone();
    }

    for(int i = 0; i < zedNum; i++)
    {
        senseData->zed_rgb[i] = zed_rgb[i].clone();
        senseData->zed_depth[i] = zed_v_depth[i].clone();
        senseData->zed_xyz[i] = zed_point_cloud[i].clone();
    }
}

cv::Mat slMat2cvMat(sl::Mat& input)
{
    // Mapping between MAT_TYPE and CV_TYPE
    int cv_type = -1;
    switch (input.getDataType())
    {
        case MAT_TYPE::F32_C1: cv_type = CV_32FC1; break;
        case MAT_TYPE::F32_C2: cv_type = CV_32FC2; break;
        case MAT_TYPE::F32_C3: cv_type = CV_32FC3; break;
        case MAT_TYPE::F32_C4: cv_type = CV_32FC4; break;
        case MAT_TYPE::U8_C1: cv_type = CV_8UC1; break;
        case MAT_TYPE::U8_C2: cv_type = CV_8UC2; break;
        case MAT_TYPE::U8_C3: cv_type = CV_8UC3; break;
        case MAT_TYPE::U8_C4: cv_type = CV_8UC4; break;
        default: break;
    }

    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM::CPU));
}
