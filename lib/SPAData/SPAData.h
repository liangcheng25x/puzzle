#ifndef SPADATA_H
#define SPADATA_H

#include <opencv2/opencv.hpp>
#include "HiwinSDK.h"
#include <sl/Camera.hpp>
#include <yaml-cpp/yaml.h>

//realsense
typedef struct
{
    std::string id;
    int width;
    int height;
}rs_init_info;

//Sense
/*typedef struct
{
    std::vector<rs_init_info> rs_init;
    std::vector<sl::InitParameters> zed_init;
}SensePara;*/

typedef struct
{
    std::vector<cv::Mat> rs_rgb;
    std::vector<cv::Mat> rs_depth;
    std::vector<cv::Mat> rs_xyz;

    std::vector<cv::Mat> zed_rgb;
    std::vector<cv::Mat> zed_depth;
    std::vector<cv::Mat> zed_xyz;
}SenseData;

//Plan

typedef struct
{
    std::vector<std::array<double, 3>> sucker_coord;
    std::array<double, 3> camera_coord;
    std::vector<std::string> sample;
}PlanPara;

typedef struct
{
    bool sw;

    HiwinSDK::CoordType coordType;
    HiwinSDK::MoveType moveType;
    HiwinSDK::CtrlType ctrlType;

    double feedRate;

    std::vector<double> value;
}ArmActting;

typedef struct
{
    bool sw;
    bool oc;
    int value;
}SerialActting;

typedef struct
{
    ArmActting arm;
    std::vector<SerialActting> sucker;
}Action;

typedef struct
{
    std::vector<Action> action;
}PlanData;

//Act
typedef struct
{
    std::string ip;
    int port;

    int operationMode;

    int accel;
    int feedRate;
    int ptpSpeed;
    double linSpeed;

    int baseNum;
    std::array<double, 6> base;
    int toolNum;
    std::array<double, 6> tool;
}ArmSetting;

typedef struct
{
    std::string port;
    std::vector<int> sucker_pin;
}SerialSetting;

typedef struct
{
    ArmSetting arm;
    SerialSetting serial_controller;
}ActPara;

typedef struct
{
    std::vector<double> currentPos;
}ActData;

#endif
