#include "surf_cf.h"
#include <vector>
#include <future>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

Surf_CF::Surf_CF()
{
    cout << "Surf_CF()" << endl;
    detector = SURF::create( 800 );
    matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE);
}

Surf_CF::~Surf_CF()
{
    cout << "~Surf_CF()" << endl;
}

void Surf_CF::init(std::vector<cv::Mat> samples)
{
    sample.resize(samples.size());
    sam_key_points.resize(samples.size());
    sam_descriptors.resize(samples.size());
    matches.resize(samples.size());
    list.resize(samples.size());

    for(size_t i = 0; i < samples.size(); i++)
    {
        samples[i].copyTo(sample[i]);
        detector->detectAndCompute( sample[i], noArray(), sam_key_points[i], sam_descriptors[i] );
    }
}

vector<similarity_list> Surf_CF::classify(Mat img)
{
    vector<KeyPoint> img_key_points;
    Mat img_descriptors;

    detector->detectAndCompute( img, noArray(), img_key_points, img_descriptors );

    for(size_t i = 0; i < matches.size(); i++)
    {
        matcher->match( sam_descriptors[i], img_descriptors, matches[i] );
        matches_sort(&matches[i]);

        list[i].cls = (int)i;
        list[i].similarity = 0;
        if(matches[i].size() >= 5)
            for(size_t j = 0; j < 5; j++)
                list[i].similarity += matches[i][j].distance;
        else
            for(size_t j = 0; j < matches[i].size(); j++)
                list[i].similarity += matches[i][j].distance;
    }

    similarity_list_sort();

    return list;
}

void Surf_CF::similarity_list_sort()
{
    similarity_list tmp;

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

void Surf_CF::matches_sort(vector< DMatch >* match)
{
    DMatch tmp;

    for(int i = match->size() - 1; i > 0; i--)
    {
        //cout << "i: " + to_string(i) << endl;
        for(int j = 0; j <= i - 1; j++)
        {
            //cout << "j: " + to_string(j) << endl;
            if( match->at(j).distance > match->at(j + 1).distance )
            {
                tmp = match->at(j);
                match->at(j) = match->at(j + 1);
                match->at(j + 1) = tmp;
            }
        }
    }
}
