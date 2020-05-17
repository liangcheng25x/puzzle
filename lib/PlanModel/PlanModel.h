#ifndef PLANMODEL_H
#define PLANMODEL_H

#include "SPAData.h"
#include "SenseModel.h"
#include "yolo_client.h"
#include "rgb_color_adjust.h"
#include <yaml-cpp/yaml.h>
#define OPENCV
#include <yolo_v2_class.hpp>

enum PuzzleState
{
    WAIT,
    TRANSPORT,
    FINISH
};

typedef struct
{
    cv::Mat img;
    std::array<int, 2> center;
    std::array<double, 3> xyz;
    int cls;
    double angle;
    PuzzleState state;
}fragment;

typedef struct
{
    cv::Mat img;
    int cls;
    double xyz;
    bool linked;
}specimen;

enum class MasterState
{
    INIT,
    IDENTIFY,
    PIECE,
    FINISH
};

enum PieceState
{
    TRACE,
    CATCH,
    GO_PUT,
    PUT,
    FINISH
}

class PlanModel
{
public:
    PlanModel(const YAML::Node& plan);
    ~PlanModel();

    void run(SenseData* senseData, PlanData* palnData);
    bool is_finish();

private:
    //fsm
    MasterState master;
    int slave;
    bool finish;
    void switchMaster(MasterState master);
    void setState(MasterState master, int slave);

    //work space
    cv::Rect roi;
    
    //coordinate
    std::vector<std::array<double, 3>> sucker_coord;
    std::array<double, 3> camera_coord;
    
    //puzzle
    int thres;
    std::vector<specimen> sample;
    std::vector<fragment> fragments;

    //master fsm
    void state_init(SenseData* senseData, PlanData* planData);
    void state_identify(SenseData* senseData, PlanData* planData);
    void state_piece(SenseData* senseData, PlanData* planData);
    void state_finish(SenseData* senseData, PlanData* planData);
};

#endif
