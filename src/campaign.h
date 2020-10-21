#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include "array.h"
#include "string.h"
#include "node_graph.h"
#include "timer.h"
#include "oscillating_timer.h"
#include "camera.h"
#include "room.h"

enum class CampaignState
{
    Invalid,
    MapSelection,
    TransitionIntoMap,
    InMap,
};

struct Campaign
{
    CampaignState state;

    Id<Arena> arena_id;

    WorkQueue *map_generation_work_queue;
    bool generation_finished[3];
    int restart_counts[3];
    float max_speeds[3];
    NodeGraph maps[3];
    GenerateNodeGraph_Params generation_params_template;
    int selected_map_index;

    Timer map_zoom_timer;
    Camera start_camera;
    Camera end_camera;

    Vec2f start_node_pos;

    OscillatingTimer node_pulse_timer;

    Array<Room> rooms;
    UnitSet player_party;
};

//Node *AddNode(Campaign *campaign, Node node, Array<int> edge_indices = Array<int>{});
void InitCampaign(Campaign *campaign);
GameState TickCampaign(Campaign *campaign);


#endif