#include <iostream>
#include "SPAData.h"
#include "PlanModel.h"

using namespace std;
using namespace cv;

void PlanModel::piece_decision(SenseData* senseData, PlanData* planData, ActData* actData)
{
    if(fragments.empty()) {
        switchMaster(MasterState::FINISH);
    }
    else {
        slave = TRACE;
    }

    //act
    planData->action.clear();
}

void PlanModel::piece_trace(SenseData* senseData, PlanData* planData, ActData* actData)
{
//     int i = 0;
//     while(i < fragments.size())
//     {
//         if(fragments[i].state == WAIT)
//             break;
//         else if(fragments[i].state == FINISH)
//             i++;
//     }

    Action trace;
    trace.arm.sw = 1;
    trace.arm.coordType = HiwinSDK::CoordType::Coord;
    trace.arm.moveType = HiwinSDK::MoveType::Relative;
    trace.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    trace.arm.feedRate = 10;
    trace.arm.value = {fragments.back().xyz[0] - actData->currentPos[0], fragments.back().xyz[1] - actData->currentPos[1], (fragments.back().xyz[2] - actData->currentPos[2] - sucker_coord[0][2]) / 2, 0, 0, 0};

    //sucker
    trace.sucker[0].sw = 0;

//     fragments[-1].state = TRANSPORT;

    planData->action.clear();
    planData->action.push_back(trace);

    slave = CATCH;
}

void PlanModel::piece_catch(SenseData* senseData, PlanData* planData, ActData* actData)
{
    //arm
    Action catch_down;
    catch_down.arm.sw = 1;
    catch_down.arm.coordType = HiwinSDK::CoordType::Coord;
    catch_down.arm.moveType = HiwinSDK::MoveType::Relative;
    catch_down.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    catch_down.arm.feedRate = 10;
    catch_down.arm.value = {-sucker_coord[0][0], -sucker_coord[0][1], senseData->rs_xyz[0].at<Vec3f>(senseData->rs_xyz[0].rows / 2, senseData->rs_xyz[0].cols / 2)[2] - sucker_coord[0][2], 0, 0, fragments.back().angle};

    //sucker
    catch_down.sucker[0].sw = 1;
    catch_down.sucker[0].oc = 1;

    //arm
    Action catch_up;
    catch_up.arm.sw = 1;
    catch_up.arm.coordType = HiwinSDK::CoordType::Coord;
    catch_up.arm.moveType = HiwinSDK::MoveType::Relative;
    catch_up.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    catch_up.arm.feedRate = 10;
    catch_up.arm.value = {0, 0, -senseData->rs_xyz[0].at<Vec3f>(senseData->rs_xyz[0].rows / 2, senseData->rs_xyz[0].cols / 2)[2] + sucker_coord[0][2], 0, 0, -fragments.back().angle};

    //sucker
    catch_up.sucker[0].sw = 0;

    planData->action.clear();
    planData->action.push_back(catch_down);
    planData->action.push_back(catch_up);

    slave = GO_PUT;
}

void PlanModel::piece_go_put(SenseData* senseData, PlanData* planData, ActData* actData)
{
//     int i = 0;
//     while(1)
//     {
//         if(fragments[i].state == TRANSPORT)
//             break;
//         else if(fragments[i].state == FINISH)
//             i++;
//     }

    Action go_put;
    go_put.arm.sw = 1;
    go_put.arm.coordType = HiwinSDK::CoordType::Coord;
    go_put.arm.moveType = HiwinSDK::MoveType::Relative;
    go_put.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    go_put.arm.feedRate = 10;
    go_put.arm.value = {sample[ fragments.back().cls ].xyz[0] - actData->currentPos[0], sample[fragments.back().cls].xyz[1] - actData->currentPos[1], 0, 0, 0, 0};

    //sucker
    go_put.sucker[0].sw = 0;

    slave = PUT;
}

void PlanModel::piece_put(SenseData* senseData, PlanData* planData, ActData* actData)
{
//     int i = 0;
//     while(i < fragments.size())
//     {
//         if(fragments[i].state == WAIT)
//             break;
//         else if(fragments[i].state == FINISH)
//             i++;
//     }

    Action put;
    put.arm.sw = 1;
    put.arm.coordType = HiwinSDK::CoordType::Coord;
    put.arm.moveType = HiwinSDK::MoveType::Relative;
    put.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    put.arm.feedRate = 10;
    put.arm.value = {-sucker_coord[0][0], -sucker_coord[0][1], senseData->rs_xyz[0].at<Vec3f>(senseData->rs_xyz[0].rows / 2, senseData->rs_xyz[0].cols / 2)[2] - sucker_coord[0][2], 0, 0, 0};

    //sucker
    put.sucker[0].sw = 1;
    put.sucker[0].oc = 0;
    
    //arm
    Action go_up;
    go_up.arm.sw = 1;
    go_up.arm.coordType = HiwinSDK::CoordType::Coord;
    go_up.arm.moveType = HiwinSDK::MoveType::Relative;
    go_up.arm.ctrlType = HiwinSDK::CtrlType::Linear;
    go_up.arm.feedRate = 10;
    go_up.arm.value = {0, 0, -senseData->rs_xyz[0].at<Vec3f>(senseData->rs_xyz[0].rows / 2, senseData->rs_xyz[0].cols / 2)[2] + sucker_coord[0][2], 0, 0, 0};

    //sucker
    go_up.sucker[0].sw = 0;

    planData->action.clear();
    planData->action.push_back(put);
    planData->action.push_back(go_up);

    fragments.pop_back();

    slave = DECISION;
}

void PlanModel::piece_finish(SenseData* senseData, PlanData* planData, ActData* actData)
{
    switchMaster(MasterState::FINISH);
}
