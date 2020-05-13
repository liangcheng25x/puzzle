#ifndef RGB_COLOR_ADJUST_H
#define RGB_COLOR_ADJUST_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <yaml-cpp/yaml.h>
#include "hist.h"

typedef struct
{
    double preBeta;
    double alpha;
    double postBeta;
    double similarity;
}ls_list;

ls_list calc_similar(cv::Mat src, std::vector<cv::Mat> sam_hist, double preBeta, double alpha, double postBeta);

class RGB_COLOR_ADJUST
{
public:
    RGB_COLOR_ADJUST();
    ~RGB_COLOR_ADJUST();

    void load_image(cv::Mat src, cv::Mat sample);

    ls_list detecting();

private:
    cv::Mat src;
    cv::Mat sample;

    //histogram
    HIST hist;

    std::vector<cv::Mat> src_hist;
    std::vector<cv::Mat> sam_hist;

    std::vector<ls_list> list;

    void ls_list_sort();
};

namespace YAML
{
    template<>
    struct convert<ls_list>
    {
        static Node encode(const ls_list& list)
        {
            Node node;

            node["preBeta"] = list.preBeta;
            node["alpha"] = list.alpha;
            node["postBeta"] = list.postBeta;

            return node;
        }

        static bool decode(const Node& node, ls_list& list)
        {
            if(node["preBeta"])
                list.preBeta = node["preBeta"].as<double>();
            else
                list.preBeta = 0;
            
            if(node["alpha"])
                list.alpha = node["alpha"].as<double>();
            else
                list.alpha = 0;
            
            if(node["postBeta"])
                list.postBeta = node["postBeta"].as<double>();
            else
                list.postBeta = 0;
            
            return true;
        }
    };
};

#endif
