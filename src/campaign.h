#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include "array.h"
#include "string.h"

struct Node
{
    Vec2f pos;
    Vec2f vel;
};

struct Edge
{
    int indices[2];
    float rest_length = c::fdg_edge_length;
};

struct Campaign
{
    Node *root;
    Array<Node> nodes;
    Array<Edge> edges;

    int end_index;
    int drag_start_index;
    bool fdg_running;

    Vec2f camera_offset;

    Timer generation_timer;
    bool generation_finished;
    float max_speed;
    bool graph_fully_connected = true;
    int generation_count;
};

Node *AddNode(Campaign *campaign, Node node, Array<int> edge_indices = Array<int>{});
void InitCampaign(Campaign *campaign);
GameState TickCampaign(Campaign *campaign);


#endif