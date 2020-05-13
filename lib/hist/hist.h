#ifndef HIST_H
#define HIST_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

class HIST
{
public:
    HIST();
    ~HIST();

    void load_image(cv::Mat src);

    void get_hist(std::vector<cv::Mat>& hist);
    void get_histImage(cv::Mat& histImage);

private:
    //histogram
    int histSize;
    float* range;
    const float** histRange;
    int hist_w, hist_h;
    int bin_w;

    cv::Mat src;
    std::vector<cv::Mat> hist;
};

#endif
