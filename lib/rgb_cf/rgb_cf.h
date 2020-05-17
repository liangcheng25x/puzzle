#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

typedef struct Similarity_List
{
    int cls;
    double similarity;
}similarity_list;

class RGB_CF
{
public:
    RGB_CF();
    ~RGB_CF();

    void init(std::vector<cv::Mat> samples);

    std::vector<similarity_list> classify(cv::Mat img);

private:
    std::vector<cv::Mat> sample;
    std::vector< std::vector< cv::Mat > > rgb_sample;
    float partition;

    //histogram
    int histSize;
    float* range;
    const float** histRange;
    std::vector< std::vector< cv::Mat > > hist;
    
    std::vector< std::vector< similarity_list > > rgb_list;
    std::vector<similarity_list> list;
    
    void similarity_list_sort();
};
