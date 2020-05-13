#include "hist.h"
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

HIST::HIST()
{
    histSize = 256;
    range = new float[2]{0, 256};
    histRange = new const float*[1] {range};
    hist_w = 512;
    hist_h = 400;
    bin_w = cvRound( (double) hist_w / histSize );
}

HIST::~HIST()
{
    delete [] range;
    delete [] histRange;
}

void HIST::load_image(Mat src)
{
    src.copyTo(this->src);

    if(src.channels() == 1)
    {
        hist.resize(src.channels());
        calcHist( &src, 1, 0, Mat(), hist[0], 1 ,&histSize, histRange, true, false );
    }
    else //channel = 3
    {
        vector<Mat> split_src;
        split( src, split_src );

        hist.resize(src.channels());
        for(size_t i = 0; i < hist.size(); i++)
        {
            calcHist( &split_src[i], 1, 0, Mat(), hist[i], 1 ,&histSize, histRange, true, false );
        }
    }
}

void HIST::get_hist(vector<Mat>& hist)
{
    hist.resize(this->hist.size());
    for(size_t i = 0; i < hist.size(); i++)
    {
        this->hist[i].copyTo(hist[i]);
    }
}

void HIST::get_histImage(Mat& histImage)
{
    histImage = Mat::zeros( hist_h, hist_w, CV_8UC3);
    
    if(hist.size() == 1)
    {
        for(size_t i = 0; i < hist.size(); i++)
        {
            normalize(hist[i], hist[i], 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        }
        
        for( int j = 1; j < histSize; j++ )
        {
            line( histImage, Point( bin_w*(j-1), hist_h - cvRound(hist[0].at<float>(j-1)) ),
                  Point( bin_w*(j), hist_h - cvRound(hist[0].at<float>(j)) ),
                  Scalar( 255, 255, 255), 2, 8, 0  );
        }
    }
    else
    {
        for(size_t i = 0; i < hist.size(); i++)
        {
            normalize(hist[i], hist[i], 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        }

        for(int j = 0; j < histSize; j++)
        {
            line( histImage, Point( bin_w*(j-1), hist_h - cvRound(hist[2].at<float>(j-1)) ),
                  Point( bin_w*(j), hist_h - cvRound(hist[2].at<float>(j)) ),
                  Scalar( 0, 0, 255), 2, 8, 0  );
            line( histImage, Point( bin_w*(j-1), hist_h - cvRound(hist[1].at<float>(j-1)) ),
                  Point( bin_w*(j), hist_h - cvRound(hist[1].at<float>(j)) ),
                  Scalar( 0, 255, 0), 2, 8, 0  );
            line( histImage, Point( bin_w*(j-1), hist_h - cvRound(hist[0].at<float>(j-1)) ),
                  Point( bin_w*(j), hist_h - cvRound(hist[0].at<float>(j)) ),
                  Scalar( 255, 0, 0), 2, 8, 0  );
        }
    }
}
