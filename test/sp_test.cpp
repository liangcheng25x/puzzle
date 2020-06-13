#include <iostream>
#include <yaml-cpp/yaml.h>
#include "SPAData.h"
#include "SenseModel.h"
#include "PlanModel.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    YAML::Node config = YAML::LoadFile( argv[1] );
    
    SenseModel senseModel;
    senseModel.init( config["sense"] );
    SenseData senseData;
    
    PlanModel planModel;
    planModel.init( config["plan"] );
    PlanData planData;

    ActData actData;

    while(!planModel.is_finish())
    {
        senseModel.run();
        senseModel.getImgs(&senseData);

        planModel.run(&senseData, &planData, &actData);

        int kb = waitKey(10);
        if(kb == 27) break;
    }

    return 0;
}
