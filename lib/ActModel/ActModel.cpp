#include "SPAData.h"
#include "ActModel.h"
#include "HiwinSDK.h"
#include "sucker.h"

ActModel::ActModel(const YAML::Node& act)
{
    cout << "act model construct" << endl;

    //robot
    robot.connect(act["act"]["ip"].as<std::string>(), act["arm"]["port"].as<int>());
    robot.setServoState(1);

    /*robot.setActiveBaseNum(actPara.arm.baseNum);
    robot.setCurrentBaseCoord(actPara.arm.base);
    robot.setActiveToolNum(actPara.arm.toolNum);
    robot.setCurrentToolCoord(actPara.arm.tool);*/

    robot.setOperationMode(act["arm"]["operationMode"].as<int>());

    robot.setFeedRate(act["arm"]["feedRate"].as<int>());
    robot.setAcceleration(act["arm"]["accel"].as<int>());
    robot.setPTPSpeed(act["arm"]["ptpSpeed"].as<int>());
    robot.setLinSpeed(act["arm"]["linSpeed"].as<int>());

    //sucker
    s_controller = new Serial_Controller(act["serial controller"]["port"].as<string>());

    sucker_pin.clear();
    for(size_t i = 0; i < act["serial controller"]["sucker"].size(); i++)
    {
        sucker_pin.push_back(act["serial controller"]["sucker"][i].as<int>());
        s_controller->close(sucker_pin[i]);
    }
}

ActModel::~ActModel()
{
    cout << "act model destruct" << endl;

    robot.setOperationMode(0);
    robot.setServoState(0);
}

void ActModel::run(PlanData* planData, ActData* actData)
{
    for(size_t i = 0; i < planData->action.size(); i++)
    {
        if(planData->action[i].arm.sw)
            control_arm(planData->action[i].arm);

        for(size_t j = 0; j < planData->action[i].sucker.size(); j++)
            if(planData->action[i].sucker[j].sw)
                control_sucker(planData->action[i].sucker[j], j);
    }

    actData->currentPos = robot.getCurrentPosition();
}

void ActModel::control_arm(ArmActting arm)
{
    robot.setFeedRate(arm.feedRate);

    robot.move(arm.value, arm.ctrlType, arm.moveType, arm.coordType);
    robot.waitForIdle();
}

void ActModel::control_sucker(SerialActting sucker, int i)
{
    if(sucker.oc)
        this->s_controller->open(sucker_pin[i]);
    else
        this->s_controller->close(sucker_pin[i]);
}
