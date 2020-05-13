#ifndef PUZZLE_CF_H
#define PUZZLE_CF_H

#include <iostream>
#include <yaml-cpp/yaml.h>
#include <opencv2/opencv.hpp>

class Puzzle_CF
{
public:
    Puzzle_CF();
    ~Puzzle_CF();

    void init(std::string file_name);

    std::vector<similarity_list> classify(cv::Mat img);

private:
    std::vector<cv::Mat> sample;

    //surf
    cv::Ptr<cv::xfeatures2d::SURF> detector;
    std::vector<std::vector<cv::KeyPoint>> sam_key_points;
    std::vector<cv::Mat> sam_descriptors;

    cv::Ptr<cv::DescriptorMatcher> matcher;
    std::vector<std::vector< cv::DMatch >> matches;

    std::vector<similarity_list> list;

    //sort
    void similarity_list_sort();
    void matches_sort(std::vector< cv::DMatch >* match);
};

#endif
