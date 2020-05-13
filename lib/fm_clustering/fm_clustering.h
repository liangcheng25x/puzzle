#ifndef FM_CLUSTERING_H
#define FM_CLUSTERING_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

class FM_Clustering
{
public:
    FM_Clustering();
    ~FM_Clustering();

    void load_data(cv::Mat source);
    void k_means(int k);
    void regress_origin(cv::Mat& rc);

private:
    cv::Mat src;
    cv::Mat dst;
    cv::Mat sorted_dst;
    cv::Mat center;
    cv::Mat sorted_center;
    cv::Mat mask;
    
    void row_sort();
};

#endif
