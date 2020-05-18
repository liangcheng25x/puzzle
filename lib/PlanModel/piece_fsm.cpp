#include <iostream>
#include "PlanModel.h"

using namespace std;
using namespace cv;

void PlanModel::piece_trace(SenseData* senseData, PlanData* planData, ActData* actData)
{
    int i = 0;
    while(i < fragments.size())
    {
        if(fragments[i].state == WAIT)
            break;
        else if(fragments[i].state == FINISH)
            i++;
    }
    
    Action trace;
    trace.arm.sw = 1;
    trace.arm.coordType = HiwinSDK::CoordType::Coord;
    trace.arm.moveType = HiwinSDK::MoveType::Relative;
    trace.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    trace.arm.feedRate = 10;
    trace.arm.value = {fragments[i].xyz[0] - actData->currentPos[0], fragments[i].xyz[1] - actData->currentPos[1], (fragments[i].xyz[2] - actData->currentPos[2] - sucker_coord[0][2]) / 2, 0, 0, 0};

    //sucker
    trace.sucker.resize(1);
    trace.sucker[0].sw = 0;

    fragments[i].state = TRANSPORT;

    planData->action.clear();
    planData->action.push_back(trace);

    slave = CATCH;
}

void PlanModel::piece_catch(SenseData* senseData, PlanData* planData, ActData* actData)
{
    int i = 0;
    while(1)
    {
        if(fragments[i].state == TRANSPORT)
            break;
        else if(fragments[i].state == FINISH)
            i++;
    }

    //arm
    Action catch;
    catch.arm.sw = 1;
    catch.arm.coordType = HiwinSDK::CoordType::Coord;
    catch.arm.moveType = HiwinSDK::MoveType::Relative;
    catch.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    catch.arm.feedRate = 10;
    catch.arm.value = {0, 0, senseData->rs_xyz[0].at<rs_xyz[0].rows / 2, rs_xyz[0].cols / 2>[2] - sucker_coord[0][2], 0, 0, fragments[i].angle};

    //sucker
    catch.sucker.resize(1);
    catch.sucker[0].sw = 1;
    catch.sucker[0].oc = 1;
    
    //arm
    Action catch_up;
    catch_up.arm.sw = 1;
    catch_up.arm.coordType = HiwinSDK::CoordType::Coord;
    catch_up.arm.moveType = HiwinSDK::MoveType::Relative;
    catch_up.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    catch_up.arm.feedRate = 10;
    catch_up.arm.value = {0, 0, -senseData->rs_xyz[0].at<rs_xyz[0].rows / 2, rs_xyz[0].cols / 2>[2] - sucker_coord[0][2], 0, 0, -fragments[i].angle};

    //sucker
    catch_up.sucker.resize(1);
    catch_up.sucker[0].sw = 0;

    planData->action.clear();
    planData->action.push_back(catch);
    planData->action.push_back(catch_up);

    slave = GO_PUT;
}

void PlanModel::piece_go_put(SenseData* senseData, PlanData* planData, ActData* actData)
{
    int i = 0;
    while(1)
    {
        if(fragments[i].state == TRANSPORT)
            break;
        else if(fragments[i].state == FINISH)
            i++;
    }

    Action go_put;
    go_put.arm.sw = 1;
    go_put.arm.coordType = HiwinSDK::CoordType::Coord;
    go_put.arm.moveType = HiwinSDK::MoveType::Relative;
    go_put.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    go_put.arm.feedRate = 10;
    go_put.arm.value = {sample[fragments[i].cls].xyz[0] - actData->currentPos[0], sample[fragments[i].cls].xyz[1] - actData->currentPos[1], 0, 0, 0, 0};

    //sucker
    go_put.sucker.resize(1);
    go_put.sucker[0].sw = 0;

    slave = PUT;
}

void PlanModel::piece_put(SenseData* senseData, PlanData* planData, ActData* actData)
{
    int i = 0;
    while(i < fragments.size())
    {
        if(fragments[i].state == WAIT)
            break;
        else if(fragments[i].state == FINISH)
            i++;
    }

    Action put;
    put.arm.sw = 1;
    put.arm.coordType = HiwinSDK::CoordType::Coord;
    put.arm.moveType = HiwinSDK::MoveType::Relative;
    put.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    put.arm.feedRate = 10;
    put.arm.value = {0, 0, senseData->rs_xyz[0].at<rs_xyz[0].rows / 2, rs_xyz[0].cols / 2>[2] - sucker_coord[0][2], 0, 0, 0};

    //sucker
    put.sucker.resize(1);
    put.sucker[0].sw = 1;
    put.sucker[0].oc = 0;

    planData->action.clear();
    planData->action.push_back(put);

    fragments[i].state = FINISH;

    if(i == fragments.size() - 1)
        switchMaster(MasterState::FINISH);
        //slave = FINISH;
    else
        slave = trace;
}

void PlanModel::piece_finish(SenseData* senseData, PlanData* planData, ActData* actData)
{
    switchMaster(MasterState::FINISH);
}
