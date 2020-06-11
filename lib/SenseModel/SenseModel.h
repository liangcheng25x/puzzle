#ifndef SENSEMODEL_H
#define SENSEMODEL_H

#include "SPAData.h"
#define YAML_CPP
#include "realsense.h"
#include <sl/Camera.hpp>
#include <yaml-cpp/yaml.h>
#include <opencv2/opencv.hpp>

cv::Mat slMat2cvMat(sl::Mat& input);

class SenseModel
{
public:
    SenseModel();
    ~SenseModel();

    //load data
    void init(const YAML::Node& sense);
    //modify setting and updata
    void run();
    //draw something by plan data
    //void drawImgs(PlanData planData);
    //show image
    void showImgs();
    //get image by sense data
    void getImgs(SenseData* senseData);

private:
    int rsNum;
    std::vector<RealSense> rs;

    int zedNum;
    sl::Camera* zed;

    std::vector<sl::Mat> sl_rgb, sl_depth, sl_point_cloud;

    std::vector<cv::Mat> rs_rgb;
    std::vector<cv::Mat> rs_v_depth;
    std::vector<cv::Mat> rs_real_mat;

    std::vector<cv::Mat> zed_rgb;
    std::vector<cv::Mat> zed_v_depth;
    std::vector<cv::Mat> zed_point_cloud;
};

namespace YAML
{
    template<>
    struct convert<sl::InitParameters>
    {
        static bool decode(const Node& node, sl::InitParameters& param)
        {
            if(node["id"])
            {
                sl::InputType input;
                input.setFromSerialNumber(node["id"].as<unsigned int>());
                param.input = input;
            }
            if(node["resolution"])
                param.camera_resolution = (sl::RESOLUTION)node["resolution"].as<int>();
            if(node["fps"])
                param.camera_fps = node["fps"].as<int>();
            if(node["depth mode"])
                param.depth_mode = (sl::DEPTH_MODE)node["depth mode"].as<int>();
            if(node["coordinate unit"])
                param.coordinate_units = (sl::UNIT)node["coordinate unit"].as<int>();
            if(node["depth minimum distance"])
                param.depth_minimum_distance = node["depth minimum distance"].as<int>();
            if(node["sdk_gpu_id"])
                param.sdk_gpu_id = node["sdk_gpu_id"].as<int>();
            /*if(node["sdk_cuda_ctx"])
                param.sdk_cuda_ctx = node["sdk_cuda_ctx"].as<int>();*/

            return true;
        }
    };

    template<>
    struct convert<sl::RuntimeParameters>
    {
        static bool decode(const Node& node, sl::RuntimeParameters& param)
        {
            if(node["sensing mode"])
                param.sensing_mode = (sl::SENSING_MODE)node["sensing mode"].as<int>();

            if(node["enable depth"])
                param.enable_depth = node["enable depth"].as<bool>();

            if(node["confidence threshold"])
                param.confidence_threshold = node["confidence threshold"].as<int>();

            if(node["texture confidence threshold"])
                param.confidence_threshold = node["texture confidence threshold"].as<int>();

            if(node["measure3D reference frame"])
                param.measure3D_reference_frame = (sl::REFERENCE_FRAME)node["measure3D reference frame"].as<int>();

            return true;
        }
    };
}

#endif
