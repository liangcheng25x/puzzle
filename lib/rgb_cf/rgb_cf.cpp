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

void RGB_CF::init(std::vector<cv::Mat> samples)
{
    sample.resize(samples.size());
    for(size_t i = 0; i < samples.size(); i++)
    {
        samples.copyTo(sample[i])
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
}

vector<similarity_list> RGB_CF::classify(cv::Mat img)
{
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
