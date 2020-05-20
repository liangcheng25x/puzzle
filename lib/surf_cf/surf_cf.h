#ifndef SURF_CF_H
#define SURF_CF_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

typedef struct Similarity_List
{
    int cls;
    double similarity;
}similarity_list;

class Surf_CF
{
public:
    Surf_CF();
    ~Surf_CF();

    void init(std::vector<cv::Mat> samples);

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
