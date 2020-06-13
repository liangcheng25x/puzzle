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

    template<>
    struct convert<cv::Rect>
    {
        static bool decode(const Node& node, cv::Rect& rect)
        {
            if( node["x"] && node["y"] && node["w"] && node["h"])
            {
                rect = cv::Rect( cv::Point( node["x"].as<int>(), node["y"].as<int>() ), cv::Size( node["w"].as<int>(), node["h"].as<int>() ) );
                return true;
            }
            else
                return false;
        }
        
        static Node encode(const cv::Rect& rect)
        {
            Node node;
            node.SetStyle(EmitterStyle::Flow);
            node["x"] = rect.x;
            node["y"] = rect.y;
            node["w"] = rect.width;
            node["h"] = rect.height;
            return node;
        }
    };
}

#endif
