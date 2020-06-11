#include <iostream>
#include <fstream>
#include "SPAData.h"
#include "SenseModel.h"
#include "PlanModel.h"
#define OPENCV
#include <yolo_v2_class.hpp>
#include "yaml.h"

using namespace std;
using namespace cv;

PlanModel::PlanModel()
{
    cout << "plan model contruct" << endl;

    //fsm
    switchMaster(MasterState::INIT);
    finish = 0;
}

PlanModel::~PlanModel()
{
    cout << "plan model destruct" << endl;
}

void PlanModel::init(const YAML::Node& plan)
{
    //work
    work_posture = plan["work posture"].as<vector<double>>();
    roi = Rect(Point( plan["work space"]["x"].as<int>(), plan["work space"]["y"].as<int>() ), Size( plan["work space"]["w"].as<int>(), plan["work space"]["h"].as<int>() ));

    //coordinate
    sucker_coord = plan["sucker_coord"].as< vector< array<double, 3> > >();

    //puzzle sample
    fragments.clear();
    
    sample.clear();
    for(size_t i = 0; i < plan["sample"].size(); i++)
    {
        specimen s;
        s.img = plan["sample"][i].as<Mat>();
        sample.push_back(s);
    }

    //image process
    thres = plan["threshold"].as<int>();
}

void PlanModel::run(SenseData* senseData, PlanData* planData, ActData* actData)
{
    switch(master)
    {
        case MasterState::INIT:
            cout << "master init" << endl;

            state_init(senseData, planData, actData);
            break;

        case MasterState::IDENTIFY:
            cout << "master identify" << endl;

            state_identify(senseData, planData, actData);
            break;

        case MasterState::PIECE:
            cout << "master piece" << endl;

            state_piece(senseData, planData, actData);
            break;

        case MasterState::FINISH:
            cout << "master finish" << endl;

            state_finish(senseData, planData, actData);
            break;
    }
}

void PlanModel::switchMaster(MasterState master)
{
    setState(master, 0);
}

void PlanModel::setState(MasterState master, int slave)
{
    this->master = master;
    this->slave = slave;
}

bool PlanModel::is_finish()
{
    return finish;
}
