#include <iostream>
#include <yaml-cpp/yaml.h>
#include "SPAData.h"
#include "SenseModel.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    YAML::Node config = YAML::LoadFile( argv[1] );
    
    SenseData senseData;
    SenseModel senseModel( config["sensePara"] );
    
    while(1)
    {
        senseModel.run();
        senseModel.showImgs();

        int kb = waitKey(10);
        if(kb == 27)
            break;
    }
    
    return 0;
}
