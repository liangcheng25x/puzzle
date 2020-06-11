#include "cf.h"

using namespace std;
using namespace cv;

Gray_CF::Gray_CF()
{
    cout << "Gray_CF()" << endl;
    histSize = 256;
    range = new float[2]{0, 256};
    histRange = new const float*[1] {range};
    partition = 4;
}

Gray_CF::~Gray_CF()
{
    cout << "~Gray_CF()" << endl;
    delete [] range;
    delete [] histRange;
}

void Gray_CF::init(std::vector<cv::Mat> samples)
{
    sample.resize(samples.size());
    
    for(size_t i = 0; i < sample.size(); i++)
    {
        samples[i].copyTo(sample[i]);
        cvtColor( sample[i], sample[i], COLOR_BGR2GRAY);
        equalizeHist(sample[i], sample[i]);

//         Rect roi( (int)(sample[i].cols / partition), (int)(sample[i].rows / partition), (int)(sample[i].cols / partition * (partition - 2)), (int)(sample[i].rows / partition * (partition - 2)) );
//         sample[i] = Mat(sample[i], roi);
    }

    hist.resize(sample.size());

    for(size_t i = 0; i < sample.size(); i++)
    {
        calcHist( &sample[i], 1, 0, Mat(), hist[i], 1, &histSize, histRange, true, false );
    }
}

vector<cls_info> Gray_CF::classify(cv::Mat img)
{
    cvtColor(img, img, COLOR_BGR2GRAY);
    equalizeHist(img, img);

//     Rect roi( (int)(img.cols / partition), (int)(img.rows / partition), (int)(img.cols / partition * (partition - 2)), (int)(img.rows / partition * (partition - 2)) );
//     img = Mat(img, roi);

    Mat img_hist;
    calcHist( &img, 1, 0, Mat(), img_hist, 1 ,&histSize, histRange, true, false );

    list.clear();
    for(unsigned int i = 0; i < sample.size(); i++)
    {
        cls_info tmp = {(int)i, compareHist(img_hist, hist[i], HISTCMP_CORREL)};
        list.push_back(tmp);
    }

    class_info_sort(&list, DESCENDING);

    return list;
}
