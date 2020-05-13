#include "fm_clustering.h"

using namespace std;
using namespace cv;

FM_Clustering::FM_Clustering()
{
    
}

FM_Clustering::~FM_Clustering()
{
    
}

void FM_Clustering::load_data(cv::Mat source)
{
    mask = Mat(source.size(), CV_8U);

    source.reshape(0, source.rows * source.cols).copyTo(src);
}

void FM_Clustering::k_means(int k)
{
    kmeans(src, k, dst, TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0), 1, KMEANS_RANDOM_CENTERS, center);

    center.copyTo(sorted_center);
    dst.copyTo(sorted_dst);
    row_sort();

    for(int i = 0; i < center.rows; i++)
    {
        for(int j = 0; j < sorted_center.rows; j++)
        {
            if(center.at<float>(i, 0) == sorted_center.at<float>(j, 0))
            {
                for(int k = 0; k < dst.rows; k++)
                    if(dst.at<int>(k, 0) == i)
                        sorted_dst.at<int>(k, 0) = j;

                continue;
            }
        }
    }
}

void FM_Clustering::regress_origin(Mat& rc)
{
    for(int i = 0; i < mask.rows; i++)
    {
        for(int j = 0; j < mask.cols; j++)
        {
            mask.at<uchar>(i, j) = cvRound(255. * sorted_dst.at<int>(i * mask.cols + j, 0) / (center.rows - 1));
        }
    }

    mask.copyTo(rc);
}

void FM_Clustering::row_sort()
{
    float tmp;
    
    for(int i = sorted_center.rows - 1; i > 0; i--)
    {
        for(int j = 0; j <= i - 1; j++)
        {
            if( sorted_center.at<float>(j, 0) > sorted_center.at<float>(j + 1, 0))
            {
                tmp = sorted_center.at<float>(j, 0);
                sorted_center.at<float>(j, 0) = sorted_center.at<float>(j + 1, 0);
                sorted_center.at<float>(j + 1, 0) = tmp;
            }
        }
    }
}
