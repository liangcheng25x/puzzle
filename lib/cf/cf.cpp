#include "cf.h"
#include <cmath>

using namespace std;
using namespace cv;

void class_info_sort(std::vector<cls_info>* list, Arrangement type)
{
    cls_info tmp;
    
    if(type == ASCENDING)
    {
        for(size_t i =list->size() - 1; i > 0; i--)
        {
            for(size_t j = 0; j <= i - 1; j++)
            {
                if( list->at(j).sl > list->at(j+1).sl)
                {
                    tmp = list->at(j);
                    list->at(j) = list->at(j+1);
                    list->at(j+1) = tmp;
                }
            }
        }
    }
    else //type = DESCENDING
    {
        for(size_t i = list->size() - 1; i > 0; i--)
        {
            for(size_t j = 0; j <= i - 1; j++)
            {
                if( list->at(j).sl < list->at(j+1).sl)
                {
                    tmp = list->at(j);
                    list->at(j) = list->at(j+1);
                    list->at(j+1) = tmp;
                }
            }
        }
    }
}
