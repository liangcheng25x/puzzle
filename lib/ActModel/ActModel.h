#ifndef ACTMODEL_H
#define ACTMODEL_H

#include "HiwinSDK.h"
#include "SPAData.h"
#include "serialController.h"
#include <yaml-cpp/yaml.h>

class ActModel
{
public:
    ActModel(const YAML::Node& act);
    ~ActModel();
    
    void run(PlanData* planData, ActData* actData);
private:
    HiwinSDK robot;
    Serial_Controller* s_controller;
    std::vector<int> sucker_pin;

    void control_arm(ArmActting arm);
    void control_sucker(SerialActting sucker, int i);
};

#endif
