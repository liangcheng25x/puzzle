#ifndef YAML_H
#define YAML_H

#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>

namespace YAML
{
    template<>
    struct convert<cv::Mat>
    {
        static bool decode(const Node& node, cv::Mat& img)
        {
            img = cv::imread( node.as<std::string>());
            return true;
        }
    };
}

#endif
