#include "rgb_cf.h"
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

RGB_CF::RGB_CF()
{
    cout << "RGB_CF()" << endl;
    histSize = 256;
    range = new float[2]{0, 256};
    histRange = new const float*[1] {range};
    partition = 4;
}

RGB_CF::~RGB_CF()
{
    cout << "~RGB_CF()" << endl;
    delete [] range;
    delete [] histRange;
}

void RGB_CF::init(string file_name )
{
    YAML::Node config = YAML::LoadFile( file_name );
    
    sample.resize( config["sample"].size() );
    for(size_t i = 0; i < config["sample"].size(); i++)
    {
        sample[i] = config["sample"][i].as<Mat>();

        for( int y = 0; y < sample[i].rows; y++ )
        {
            for( int x = 0; x < sample[i].cols; x++ )
            {
                for( int z = 0; z < sample[i].channels(); z++)
                {
                    sample[i].at<Vec3b>(y,x)[z] = saturate_cast<uchar>( 0.41 * sample[i].at<Vec3b>(y,x)[z] + 21 );
                }
            }
        }
        //         Rect roi( (int)(sample[i].cols / partition), (int)(sample[i].rows / partition), (int)(sample[i].cols / partition * (partition - 2)), (int)(sample[i].rows / partition * (partition - 2)) );
        //         sample[i] = Mat(sample[i], roi);
    }
    
    rgb_sample.resize(sample.size());
    hist.resize(sample.size());
    
    for(size_t i = 0; i < sample.size(); i++)
    {
        split( sample[i], rgb_sample[i]);
    }
    
    for(size_t i = 0; i < sample.size(); i++)
    {
        hist[i].resize( rgb_sample[i].size() );
        
        for(size_t j = 0;j < rgb_sample[i].size(); j++)
        {
            calcHist( &rgb_sample[i][j], 1, 0, Mat(), hist[i][j], 1, &histSize, histRange, true, false );
        }
    }
    
    /*int hist_w = 512, hist_h = 400;
    int bin_w = cvRound( (double) hist_w / histSize );
    vector<Mat> histImage(sample.size());
    for(size_t i = 0; i < sample.size(); i++)
    {
        histImage[i] = Mat( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    }
    
    for(size_t i = 0; i < hist.size(); i++)
    {
        for(size_t j = 0; j < hist[i].size(); j++)
        {
            normalize(hist[i][j], hist[i][j], 0, histImage[i].rows, NORM_MINMAX, -1, Mat() );
        }
    }
    cout << "noamlized" << endl;
    
    for(size_t i = 0; i < hist.size(); i++)
    {
        for( int j = 1; j < histSize; j++ )
        {
            line( histImage[i], Point( bin_w*(j-1), hist_h - cvRound(hist[i][0].at<float>(j-1)) ),
                  Point( bin_w*(j), hist_h - cvRound(hist[i][0].at<float>(j)) ),
                  Scalar( 255, 0, 0), 2, 8, 0  );
        }
        for( int j = 1; j < histSize; j++ )
        {
            line( histImage[i], Point( bin_w*(j-1), hist_h - cvRound(hist[i][1].at<float>(j-1)) ),
                  Point( bin_w*(j), hist_h - cvRound(hist[i][1].at<float>(j)) ),
                  Scalar( 0, 255, 0), 2, 8, 0  );
        }
        for( int j = 1; j < histSize; j++ )
        {
            line( histImage[i], Point( bin_w*(j-1), hist_h - cvRound(hist[i][2].at<float>(j-1)) ),
                  Point( bin_w*(j), hist_h - cvRound(hist[i][2].at<float>(j)) ),
                  Scalar( 0, 0, 255), 2, 8, 0  );
        }
    }
    cout << "visual histogram finish" << endl;
    
    for(size_t i = 0; i < hist.size(); i++)
    {
        imshow( "img" + to_string(i), histImage[i]);
    }*/
}

vector<similarity_list> RGB_CF::classify(cv::Mat img)
{
    //     Rect roi( (int)(img.cols / partition), (int)(img.rows / partition), (int)(img.cols / partition * (partition - 2)), (int)(img.rows / partition * (partition - 2)) );
    //     img = Mat(img, roi);
    
    vector<Mat> rgb_img;
    split( img, rgb_img);
    
    vector<Mat> img_hist( rgb_img.size() );
    for(size_t i = 0; i < rgb_img.size(); i++)
    {
        calcHist( &rgb_img[i], 1, 0, Mat(), img_hist[i], 1 ,&histSize, histRange, true, false );
    }

    rgb_list.resize(hist.size());
    for(unsigned int i = 0; i < hist.size(); i++)
    {
        rgb_list[i].resize(img_hist.size());

        for(size_t j = 0;j < img_hist.size() ; j++)
        {
            similarity_list tmp = {(int)i, compareHist(img_hist[j], hist[i][j], HISTCMP_INTERSECT)};
            rgb_list[i][j] = tmp;
        }
    }
    
    list.resize( rgb_list.size() );
    for(size_t i = 0; i < rgb_list.size(); i++)
    {
        list[i].cls = (int)i;
        double tmp = 0;

        for(size_t j = 0; j < rgb_list[i].size(); j++)
        {
            tmp += pow(rgb_list[i][j].similarity, 2);
        }
        
        list[i].similarity = sqrt(tmp / rgb_img.size());
    }
    
    similarity_list_sort();
    
    /*int hist_w = 512, hist_h = 400;
    int bin_w = cvRound( (double) hist_w / histSize );
    Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    
    for(size_t j = 0; j < img_hist.size(); j++)
    {
        normalize(img_hist[j], img_hist[j], 0, histImage.rows, NORM_MINMAX, -1, Mat() );
    }
    cout << "noamlized" << endl;
    
    for( int j = 1; j < histSize; j++ )
    {
        line( histImage, Point( bin_w*(j-1), hist_h - cvRound(img_hist[0].at<float>(j-1)) ),
              Point( bin_w*(j), hist_h - cvRound(img_hist[0].at<float>(j)) ),
              Scalar( 255, 0, 0), 2, 8, 0  );
    }
    for( int j = 1; j < histSize; j++ )
    {
        line( histImage, Point( bin_w*(j-1), hist_h - cvRound(img_hist[1].at<float>(j-1)) ),
              Point( bin_w*(j), hist_h - cvRound(img_hist[1].at<float>(j)) ),
              Scalar( 0, 255, 0), 2, 8, 0  );
    }
    for( int j = 1; j < histSize; j++ )
    {
        line( histImage, Point( bin_w*(j-1), hist_h - cvRound(img_hist[2].at<float>(j-1)) ),
              Point( bin_w*(j), hist_h - cvRound(img_hist[2].at<float>(j)) ),
              Scalar( 0, 0, 255), 2, 8, 0  );
    }
    cout << "visual histogram finish" << endl;
    
    imshow( "img_hist", histImage);*/
    
    return list;
}

void RGB_CF::similarity_list_sort()
{
    similarity_list tmp;
    
    for(size_t i = list.size() - 1; i > 0; i--)
    {
        for(size_t j = 0; j <= i - 1; j++)
        {
            if( list[j].similarity < list[j+1].similarity)
            {
                tmp = list[j];
                list[j] = list[j+1];
                list[j+1] = tmp;
            }
        }
    }
}
