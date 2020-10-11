#ifndef GENERATE_NODE_GRAPH_PARAMS
#define GENERATE_NODE_GRAPH_PARAMS

#include "memory.h"
//#include "campaign.h"

struct NodeGraph;
struct Campaign;

struct GenerateNodeGraph_Params
{
    bool *thread_finished = nullptr;
    int *restart_count    = nullptr;
    float *max_speed      = nullptr;

    NodeGraph *graph = nullptr;
    u32 main_path_min = 12;
    u32 main_path_max = 16;
    int max_linear_branch_length = 3;
    float linear_branch_extension_chance = 50.f;
    int loop_generation_count = 10;
};

#endif