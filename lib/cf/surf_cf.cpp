#include "cf.h"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

Surf_CF::Surf_CF()
{
    cout << "Surf_CF()" << endl;
    detector = SURF::create( 400 );
    matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE);
}

Surf_CF::~Surf_CF()
{
    cout << "~Surf_CF()" << endl;
}

void Surf_CF::init(std::vector<cv::Mat> samples)
{
    //memory allocation
    sample.resize(samples.size());
    sam_key_points.resize(samples.size());
    sam_descriptors.resize(samples.size());
    matches.resize(samples.size());
    list.resize(samples.size());

    //copy samples and detect key point
    for(size_t i = 0; i < samples.size(); i++)
    {
        sample[i] = samples[i].clone();
        detector->detectAndCompute( sample[i], noArray(), sam_key_points[i], sam_descriptors[i] );
    }
}

vector<cls_info> Surf_CF::classify(Mat img, int accuracy)
{
    vector<KeyPoint> img_key_points;
    Mat img_descriptors;

    //detect key point of key point
    detector->detectAndCompute( img, noArray(), img_key_points, img_descriptors );

    //match key point
    for(size_t i = 0; i < matches.size(); i++)
    {
        if(sam_key_points[i].size() != 0 && img_key_points.size() != 0) {
            matcher->match( sam_descriptors[i], img_descriptors, matches[i] );
            matches_sort(&matches[i]);
            
            //according accuracy to decide how many matched key point is going to use
            list[i].cls = (int)i;
            list[i].sl = 0;
            if(matches[i].size() >= accuracy)
                for(size_t j = 0; j < accuracy; j++)
                    list[i].sl += 1 / matches[i][j].distance;
                else {
                    cout << "accuracy isn't enough" << endl;
                    for(size_t j = 0; j < matches[i].size(); j++)
                        list[i].sl += 1 / matches[i][j].distance;
                }
        }
        else {
            cout << "image hasn't keypoint: " << i << endl;
        }
    }

    class_info_sort(&list, DESCENDING);

    return list;
}

void Surf_CF::matches_sort(vector< DMatch >* match)
{
    DMatch tmp;

    if(match->size() == 0)
        return;
    
    for(size_t i = match->size() - 1; i > 0; i--)
    {
        for(size_t j = 0; j <= i - 1; j++)
        {
            if( match->at(j).distance > match->at(j + 1).distance )
            {
                tmp = match->at(j);
                match->at(j) = match->at(j + 1);
                match->at(j + 1) = tmp;
            }
        }
    }
}
