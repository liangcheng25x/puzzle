#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

static void alpha_on_trackbar( int alpha_, void* tmp)
{
    double* alpha = (double*)tmp;

    *alpha = alpha_ / 10.;
}

static void beta_on_trackbar( int beta_, void* tmp)
{
    int* beta = (int*)tmp;

    *beta = beta_ - 100;
}

static void gamma_on_trackbar( int gamma_, void* tmp)
{
    double* gamma = (double*)tmp;

    *gamma = gamma_ / 100.;
}

int main( int argc, char** argv )
{
    Mat src = imread( argv[1], IMREAD_GRAYSCALE );
    Mat img = imread(argv[2], IMREAD_GRAYSCALE );
    Mat img_1 = Mat::zeros( img.size(), img.type() );
    Mat img_2 = Mat::zeros( img.size(), img.type() );

    int alpha_ = 10;
    int beta_ = 100;
    int gamma_ = 100;

    double alpha = 1.0;
    int beta = 0;
    double gamma = 1;

    //histogram setting
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };

    Mat hist_src;
    Mat hist;
    Mat hist_1;
    Mat hist_2;
    
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound( (double) hist_w / histSize );
    Mat histImage_src( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    Mat histImage_1( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    Mat histImage_2( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    
    calcHist( &src, 1, 0, Mat(), hist_src, 1 ,&histSize, &histRange, true, false );
    normalize(hist_src, hist_src, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
    calcHist( &img, 1, 0, Mat(), hist, 1 ,&histSize, &histRange, true, false );
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
    
    for(int j = 0; j < histSize; j++)
    {
        line( histImage_src, Point( bin_w*(j-1), hist_h - cvRound(hist_src.at<float>(j-1)) ),
            Point( bin_w*(j), hist_h - cvRound(hist_src.at<float>(j)) ),
            Scalar( 255, 255, 255), 2, 8, 0  );
        line( histImage, Point( bin_w*(j-1), hist_h - cvRound(hist.at<float>(j-1)) ),
            Point( bin_w*(j), hist_h - cvRound(hist.at<float>(j)) ),
            Scalar( 255, 255, 255), 2, 8, 0  );
    }

    namedWindow("control", WINDOW_AUTOSIZE);

    createTrackbar("alpha", "control", &alpha_, 100, alpha_on_trackbar, &alpha);
    createTrackbar("beta", "control", &beta_, 200, beta_on_trackbar, &beta);
    createTrackbar("gamma", "control", &gamma_, 2500, gamma_on_trackbar, &gamma);

    while(1)
    {
        for( int y = 0; y < img.rows; y++ )
        {
            for( int x = 0; x < img.cols; x++ )
            {
                for( int c = 0; c < img.channels(); c++ )
                {
                    img_1.at<uchar>(y,x) = saturate_cast<uchar>(pow(img.at<uchar>(y, x) / 255.0, gamma) * 255.0);
                }
            }
        }
        
        for( int y = 0; y < img_1.rows; y++ )
        {
            for( int x = 0; x < img_1.cols; x++ )
            {
                img_2.at<uchar>(y,x) = saturate_cast<uchar>( alpha*img_1.at<uchar>(y,x) + beta );
            }
        }

        calcHist( &img_1, 1, 0, Mat(), hist_1, 1 ,&histSize, &histRange, true, false );
        calcHist( &img_2, 1, 0, Mat(), hist_2, 1 ,&histSize, &histRange, true, false );
        
        normalize(hist_1, hist_1, 0, histImage_1.rows, NORM_MINMAX, -1, Mat() );
        normalize(hist_2, hist_2, 0, histImage_2.rows, NORM_MINMAX, -1, Mat() );
        
        cout << "similarity: " << compareHist(hist_src, hist_2, HISTCMP_CORREL) << endl;

        for( int y = 0; y < histImage_1.rows; y++ )
        {
            for( int x = 0; x < histImage_1.cols; x++ )
            {
                for( int z = 0; z < histImage_1.channels(); z++ )
                {
                    histImage_1.at<Vec3b>(y, x)[z] = 0;
                    histImage_2.at<Vec3b>(y, x)[z] = 0;
                }
            }
        }
        
        for(int j = 0; j < histSize; j++)
        {
            line( histImage_1, Point( bin_w*(j-1), hist_h - cvRound(hist_1.at<float>(j-1)) ),
                Point( bin_w*(j), hist_h - cvRound(hist_1.at<float>(j)) ),
                Scalar( 255, 255, 255), 2, 8, 0  );

            line( histImage_2, Point( bin_w*(j-1), hist_h - cvRound(hist_2.at<float>(j-1)) ),
                Point( bin_w*(j), hist_h - cvRound(hist_2.at<float>(j)) ),
                Scalar( 255, 255, 255), 2, 8, 0  );
        }

        imshow("src", src);
        imshow("img", img);
        imshow("img 1", img_1);
        imshow("img 2", img_2);
        imshow("hist src", histImage_src);
        imshow("hist", histImage);
        imshow("hist 1", histImage_1);
        imshow("hist 2", histImage_2);

        char c = waitKey(10);
        if(c == 27)
            break;
    }

    return 0;
}
 
