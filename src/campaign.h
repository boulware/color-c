#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include "array.h"
#include "string.h"
#include "node_graph.h"

struct Campaign
{
    Arena arena;

    WorkQueue *map_generation_work_queue;
    bool generation_finished[3];
    int restart_counts[3];
    float max_speeds[3];
    NodeGraph maps[3];
    GenerateNodeGraph_Params generation_params_template;
    // Node *root;
    // Array<Node> nodes;
    // Array<Edge> edges;

    // int end_index;
    // int drag_start_index;
    // bool fdg_running;

    // Vec2f camera_offset;

    // Timer generation_timer;
    // bool generation_finished;
    // float max_speed;
    // bool graph_fully_connected = true;
    // int generation_count;
};

//Node *AddNode(Campaign *campaign, Node node, Array<int> edge_indices = Array<int>{});
void InitCampaign(Campaign *campaign);
GameState TickCampaign(Campaign *campaign);


#endif