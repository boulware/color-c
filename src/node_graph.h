#ifndef NODE_GRAPH_H
#define NODE_GRAPH_H

#include "array.h"
#include "memory.h"
#include "generate_node_graph_params.h"

struct Node
{
    Vec2f pos;
    Vec2f vel;
};

struct Edge
{
    int a, b;
};

struct NodeGraph
{
    Array<Node> nodes;
    Array<Edge> edges;

    int start_index;
    int end_index;
};

struct NodeGraphResponse
{
    Vec2f start_node_pos;
};

struct ForceSimParams
{
    Arena *temp_arena = nullptr;
    float edge_free_length = 50.f;
    float spring_constant  = 0.01f;
    float charge_strength  = 1000.0f;
    float friction         = 0.99f;
    float max_speed        = 2.f;
};

enum class ForceSimInvalidationReason
{
    Unspecified,
    NotFullyConnected,
    MaxSpeedExceeded,
};

struct ForceSimState
{
    bool invalid_state;
    ForceSimInvalidationReason invalidation_reason;
    bool semi_stable;
    float max_speed;
    bool finished;
    int restart_count;
    int max_restart_count;
};

void THREAD_GenerateNodeGraph(void *data, Arena *thread_arena);

void GenerateNodeGraph(NodeGraph *graph, GenerateNodeGraph_Params params);
ForceSimState StepNodeGraphForceSimulation(NodeGraph *graph, ForceSimParams params, float dt, int iter_count);

NodeGraphResponse TransformNodeGraphPointsToFitInsideRect(NodeGraph *graph, Rect rect);
void DrawNodeGraph(NodeGraph *graph);
NodeGraphResponse DrawNodeGraphInRect(NodeGraph *graph, Rect rect);

int MaxNodesFromGenerationParams(GenerateNodeGraph_Params params);
int MaxEdgesFromGenerationParams(GenerateNodeGraph_Params params);

#endif