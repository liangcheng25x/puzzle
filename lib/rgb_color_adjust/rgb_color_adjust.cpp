#include "rgb_color_adjust.h"
#include <vector>
#include <future>
#include <array>

using namespace std;
using namespace cv;

RGB_COLOR_ADJUST::RGB_COLOR_ADJUST()
{
    cout << "RGB_COLOR_ADJUST()" << endl;
}

RGB_COLOR_ADJUST::~RGB_COLOR_ADJUST()
{
    cout << "~RGB_COLOR_ADJUST()" << endl;
}

void RGB_COLOR_ADJUST::load_image(Mat src, Mat sample)
{
    src.copyTo( this->src );
    sample.copyTo( this->sample );

    hist.load_image(sample);
    hist.get_hist(sam_hist);
}

ls_list RGB_COLOR_ADJUST::detecting()
{
    list.clear();

    array<future<ls_list>, 31> f;

    for(double pb = 0; pb <= 0; pb++)
    {
        for(double a = 0.3; a < 1; a = a + 0.01)
        {
            for(double b = -15; b <= 15; b++)
            {
                f[b + 15] = async(launch::async, calc_similar, src, sam_hist, pb, a, b);

                /*Mat tmp = Mat::zeros( src.size(), src.type() );
                vector<Mat> tmp_hist;

                for( int y = 0; y < tmp.rows; y++ )
                {
                    for( int x = 0; x < tmp.cols; x++ )
                    {
                        for( int z = 0; z < tmp.channels(); z++)
                        {
                            tmp.at<Vec3b>(y,x)[z] = saturate_cast<uchar>( a * (src.at<Vec3b>(y,x)[z] + pb) + b );
                        }
                    }
                }
                
                hist.load_image(tmp);
                hist.get_hist(tmp_hist);
                
                double sl_tmp = 0;
                for( size_t i = 0; i < tmp_hist.size(); i++)
                {
                    sl_tmp += pow(compareHist(tmp_hist[i], sam_hist[i], HISTCMP_BHATTACHARYYA), 2);
                }
                ls_list ls = {pb, a, b, sl_tmp};

                list.push_back(ls);*/
            }

            for(size_t i = 0; i < f.size(); i++)
            {
                list.push_back(f[i].get());
            }
        }

        for(double a = 1; a < 7; a = a + 0.1)
        {
            for(double b = -15; b <= 15; b++)
            {
                f[b + 15] = async(launch::async, calc_similar, src, sam_hist, pb, a, b);
                
                /*Mat tmp = Mat::zeros( src.size(), src.type() );
                vector<Mat> tmp_hist;
                
                for( int y = 0; y < tmp.rows; y++ )
                {
                    for( int x = 0; x < tmp.cols; x++ )
                    {
                        for( int z = 0; z < tmp.channels(); z++)
                        {
                            tmp.at<Vec3b>(y,x)[z] = saturate_cast<uchar>( a * (src.at<Vec3b>(y,x)[z] + pb) + b );
                        }
                    }
                }
                
                hist.load_image(tmp);
                hist.get_hist(tmp_hist);
                
                double sl_tmp = 0;
                for( size_t i = 0; i < tmp_hist.size(); i++)
                {
                    sl_tmp += pow(compareHist(tmp_hist[i], sam_hist[i], HISTCMP_BHATTACHARYYA), 2);
                }
                ls_list ls = {pb, a, b, sl_tmp};

                list.push_back(ls);*/
            }

            for(size_t i = 0; i < f.size(); i++)
            {
                list.push_back(f[i].get());
            }
        }
    }

    ls_list_sort();

    return list[0];
}

void RGB_COLOR_ADJUST::ls_list_sort()
{
    ls_list tmp;

    for(size_t i = list.size() - 1; i > 0; i--)
    {
        for(size_t j = 0; j <= i - 1; j++)
        {
            if( list[j].similarity > list[j+1].similarity)
            {
                tmp = list[j];
                list[j] = list[j+1];
                list[j+1] = tmp;
            }
        }
    }
}

ls_list calc_similar(Mat src, vector<Mat> sam_hist, double preBeta, double alpha, double postBeta)
{
    Mat tmp = Mat::zeros( src.size(), src.type() );
    vector<Mat> tmp_hist;

    for( int y = 0; y < tmp.rows; y++ )
    {
        for( int x = 0; x < tmp.cols; x++ )
        {
            for( int z = 0; z < tmp.channels(); z++)
            {
                tmp.at<Vec3b>(y,x)[z] = saturate_cast<uchar>( alpha * (src.at<Vec3b>(y,x)[z] + preBeta) + postBeta );
            }
        }
    }

    HIST hist;
    hist.load_image(tmp);
    hist.get_hist(tmp_hist);

    double sl_tmp = 0;
    for( size_t i = 0; i < tmp_hist.size(); i++)
    {
        sl_tmp += pow(compareHist(tmp_hist[i], sam_hist[i], HISTCMP_BHATTACHARYYA), 2);
    }
    ls_list ls = {preBeta, alpha, postBeta, sl_tmp};

    return ls;
}
