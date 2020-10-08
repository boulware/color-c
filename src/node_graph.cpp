#include "node_graph.h"

bool
EdgeAlreadyExists(NodeGraph *graph, Edge edge)
{
    for(auto other : graph->edges)
    {
        bool same_index = other.a == edge.a and other.b == edge.b;
        bool opp_index  = other.a == edge.b and other.b == edge.a;

        if(same_index or opp_index) return true;
    }

    return false;
}

void
GenerateNodeGraph(NodeGraph *graph, NodeGraphParams param)
{
    ClearArray(&graph->nodes);
    ClearArray(&graph->edges);

    // Main path
    u32 main_path_node_count = RandomU32(param.main_path_min, param.main_path_max);
    ResizeArray(&graph->nodes, main_path_node_count);
    for(int i=0; i<main_path_node_count; ++i)
    {
        Node node = {
            .pos = {400.f + 10.f*i, 450.f}
        };
        graph->nodes += node;
    }

    graph->start_index = 0;
    graph->end_index   = graph->nodes.count-1;

    for(int i=1; i<graph->nodes.count; ++i)
    {
        Edge edge = {.a=i-1, .b=1};
        //edge.rest_length = c::fdg_edge_length + RandomFloat(-30.f,30.f);
        if(!EdgeAlreadyExists(graph, edge)) graph->edges += edge;
    }

    // Linear branches

    int j = 0;
    int cur_branch = 0;
    int max_branch = 3;
    for(int i=0; i<main_path_node_count; ++i)
    {
        ++j;
        if(Chance(150.0f/m))
        {
            Node node = {
                .pos = {410.f + 1.f*j, 460.f+1.f*j},
                .vel = {0.f,0.f}//{(float)RandomU32(0,2)-1, (float)RandomU32(0,2)-1}
            };
            graph->nodes += node;

            //int attach_index = RandomU32(1, main_path_node_count-2);
            Edge edge = {};
            edge.a = graph->nodes.count-1;
            edge.b = i;//attach_index;
            edge.rest_length = c::fdg_edge_length + RandomFloat(-30.f,30.f);
            if(!EdgeAlreadyExists(graph, edge)) graph->edges += edge;

            if(++cur_branch >= max_branch) break;
            --i;
        }
    }

    int start_count = graph->nodes.count;
    for(int i=0; i<m*5; ++i)
    {
        ++j;
        //if(Chance(50.0f))
        {
            Node node = {
                .pos = {410.f + 1.f*j, 460.f+1.f*j},
                .vel = {0.f,0.f}//{(float)RandomU32(0,2)-1, (float)RandomU32(0,2)-1}
            };
            graph->nodes += node;

            Edge edge = {};
            edge.a = graph->nodes.count-1;
            edge.b = RandomU32(0, graph->nodes.count-2);;//attach_index;
            if(!EdgeAlreadyExists(graph, edge)) graph->edges += edge;

            edge.a = graph->nodes.count-1;
            edge.b = RandomU32(0, graph->nodes.count-2);;//attach_index;
            if(!EdgeAlreadyExists(graph, edge)) graph->edges += edge;
        }
    }
}