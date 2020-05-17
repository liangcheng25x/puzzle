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

PlanModel::PlanModel(const YAML::Node& plan)
{
    cout << "plan model contruct" << endl;

    //fsm
    switchMaster(MasterState::INIT);
    finish = 0;

    //work space
    roi = Rect(Point( config["work space"]["x"].as<int>(), config["work space"]["y"].as<int>() ), Size( config["work space"]["w"].as<int>(), config["work space"]["h"].as<int>() ));

    //coordinate
    sucker_coord.clear();
    for(size_t i = 0; i < plan["sucker_coord"].size(); i++)
        sucker_coord = plan["sucker_coord"][i].as<array<double, 3>>();
    camera_coord = plan["camera_coord"].as<array<double, 3>>();

    //puzzle sample
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

PlanModel::~PlanModel()
{
    cout << "plan model destruct" << endl;
}

void PlanModel::run(SenseData* senseData, PlanData* planData)
{
    switch(master)
    {
        case MasterState::INIT:
            cout << "master init" << endl;

            state_init(senseData, planData);
            break;

        case MasterState::IDENTIFY:
            cout << "master identify" << endl;

            state_identify(senseData, planData);
            break;

        case MasterState::PIECE:
            cout << "master piece" << endl;

            state_piece(senseData, planData);
            break;

        case MasterState::FINISH:
            cout << "master finish" << endl;

            state_finish(senseData, planData);
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
