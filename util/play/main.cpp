#include "SPAData.h"
#include "SenseModel.h"
#include "PlanModel.h"
#include "ActModel.h"
#include "yamlParser.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    SensePara sensePara = {};
    SenseData senseData = {};

    PlanPara planPara = {};
    PlanData planData = {};

    ActPara actPara = {};
    ActData actData = {};

    paraSetting(&sensePara, &planPara, &actPara, argv[1]);
    SenseModel senseModel(sensePara);
    PlanModel planModel(planPara);
    ActModel actModel(actPara);
    char c = 0;
    
    while(c != 27 && !planModel.is_finish())
    {
        senseModel.run();
        senseModel.getImgs(&senseData);
        planModel.run(&senseData, &planData);
        senseModel.drawImgs(planData);
        senseModel.showImgs();
        actModel.run(&planData, &actData);

        c = waitKey(10);
    }
    
    return 0;
}
