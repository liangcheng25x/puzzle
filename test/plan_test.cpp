#include <iostream>
#include <yaml-cpp/yaml.h>
#include "SPAData.h"
#include "SenseModel.h"
#include "PlanModel.h"
#include "yaml.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    YAML::Node node = YAML::LoadFile( argv[1] );

    SenseModel senseModel;
    senseModel.init(node["sense"]);
    SenseData senseData;

    PlanModel planModel;
    planModel.init(node["plan"]);
    PlanData planData;

    ActData actData;

    while(!planModel.is_finish())
    {
        senseModel.run();
        senseModel.getImgs(&senseData);

        planModel.run(&senseData, &planData, &actData);
    }

    return 0;
}
