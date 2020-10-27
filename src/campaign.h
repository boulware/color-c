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
    InRoom,
};

struct Campaign
{
    CampaignState state;

    PoolId<Arena> arena_id;
    PoolId<Arena> battle_arena_id;

    WorkQueue *map_generation_work_queue;
    bool show_generation[3];
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

    bool room_is_init;
    Array<Room> rooms;
    int current_room_index;
    UnitSet player_party;

    Camera map_camera;

    Battle current_battle;
};

//Node *AddNode(Campaign *campaign, Node node, Array<int> edge_indices = Array<int>{});
void InitCampaign(Campaign *campaign);
GameState TickCampaign(Campaign *campaign);


#endif