#ifndef CF_H
#define CF_H

#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>

//Record category and similarity
typedef struct
{
    int cls;
    double sl;
}cls_info;

enum Arrangement
{
    ASCENDING,
    DESCENDING
};

void class_info_sort(std::vector<cls_info>* list, Arrangement type);

class Gray_CF
{
public:
    Gray_CF();
    ~Gray_CF();

    //load sample
    void init(std::vector<cv::Mat> samples);

    //classify
    std::vector<cls_info> classify(cv::Mat img);

private:
    //samples
    std::vector<cv::Mat> sample;
    //interesing region of image
    double partition;

    //histogram
    int histSize;
    float* range;
    const float** histRange;
    std::vector<cv::Mat> hist;

    //class information between loaded image and samples
    std::vector<cls_info> list;
};

class RGB_CF
{
public:
    RGB_CF();
    ~RGB_CF();

    //load sample
    void init(std::vector<cv::Mat> samples);

    //classify
    std::vector<cls_info> classify(cv::Mat img);

private:
    //samples
    std::vector<cv::Mat> sample;
    //color mapping of samples
    std::vector< std::vector< cv::Mat > > rgb_sample;
    //interesing region of image
    double partition;

    //histogram
    int histSize;
    float* range;
    const float** histRange;
    std::vector< std::vector< cv::Mat > > hist;

    //class information between loaded image and samples
    std::vector< std::vector< cls_info > > rgb_list;
    std::vector<cls_info> list;
};

class Surf_CF
{
public:
    Surf_CF();
    ~Surf_CF();

    //load sample
    void init(std::vector<cv::Mat> samples);

    //classify
    std::vector<cls_info> classify(cv::Mat img, int accuracy);

private:
    //samples
    std::vector<cv::Mat> sample;

    cv::Ptr<cv::xfeatures2d::SURF> detector;
    std::vector<std::vector<cv::KeyPoint>> sam_key_points;
    std::vector<cv::Mat> sam_descriptors;

    cv::Ptr<cv::DescriptorMatcher> matcher;
    std::vector<std::vector< cv::DMatch >> matches;

    //class information between loaded image and samples
    std::vector<cls_info> list;

    //sort
    void matches_sort(std::vector< cv::DMatch >* match);
};

#endif
