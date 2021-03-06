#include "node_graph.h"

#include "generate_node_graph_params.h"

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

bool
GraphIsFullyConnected(NodeGraph *graph, PoolId<Arena> temp_arena_id)
{
    //Node &root =  graph->nodes[0];

    ClearArena(temp_arena_id);
    Array<int> indices_to_search = CreateArrayFromArena<int>(20, temp_arena_id);
    indices_to_search += 0; // add "root"

    Array<int> searched_indices = CreateArrayFromArena<int>(20, temp_arena_id);
    Array<Edge> connected_edges = CreateArrayFromArena<Edge>(10, temp_arena_id);
    //for(int i=0; i<graph->edges.count; ++i)
    while(indices_to_search.count > 0)
    {
        ClearArray(&connected_edges);
        int this_index = indices_to_search[indices_to_search.count-1];
        indices_to_search.count -= 1;
        searched_indices += this_index;
        for(auto edge : graph->edges)
        {
            if(this_index == edge.a)
            {
                //connected_edges += edge;
                if(     !ElementInArray(&searched_indices, edge.b)
                    and !ElementInArray(&indices_to_search, edge.b))
                {
                    indices_to_search += edge.b;
                }
            }
            else if(this_index == edge.b)
            {
                //connected_edges += edge;
                if(    !ElementInArray(&searched_indices, edge.a)
                   and !ElementInArray(&indices_to_search, edge.a))
                {
                    indices_to_search += edge.a;
                }
            }
        }
    }

    return (searched_indices.count == graph->nodes.count);
}

void
CompleteNode(NodeGraph *graph, int completed_node_index)
{
    graph->nodes[completed_node_index].completed = true;

    for(int i=0; i<graph->edges.count; ++i)
    {
        Edge &edge = graph->edges[i]; // alias

        if(edge.a == completed_node_index)
        {
            graph->nodes[edge.b].reachable = true;
        }
        else if(edge.b == completed_node_index)
        {
            graph->nodes[edge.a].reachable = true;
        }
    }
}

void
THREAD_GenerateNodeGraph(void *data, PoolId<Arena> thread_arena_id)
{
    GenerateNodeGraph_Params params = *(GenerateNodeGraph_Params *)data;

//    Log("[%u] Started "  __FUNCTION__ "() thread.",  platform->GetCallingThreadId());

    // START
    if(!params.thread_finished or !params.restart_count or !params.graph) return;
    NodeGraph *graph = params.graph;

    GenerateNodeGraph(graph, params);

    // @todo: This infinite while loop is still a bit sketchy.
    ForceSimState state = {};
    while(!state.finished)
    {
        ClearArena(thread_arena_id);
        ForceSimParams force_params = {};
        force_params.temp_arena = thread_arena_id;
        int time_steps = 500;
        if(*params.max_speed <= c::speed_threshold_for_fast_timestep)
            time_steps = 100;

        state = StepNodeGraphForceSimulation(graph, force_params, 1.f/time_steps, time_steps);
        *(params.max_speed) = state.max_speed;

        ClearArena(thread_arena_id);
        if(state.invalid_state)
        {
            ++(*params.restart_count);
            GenerateNodeGraph(graph, params);
            //state.finished = true;
        }

        if(state.finished)
        {
            break;
        }
    }

    // Set reachable for each node.
    graph->nodes[graph->start_index].reachable = true;



    *(params.thread_finished) = true;
    // END

//    Log("[%u] Finished " __FUNCTION__ "() thread.", platform->GetCallingThreadId());
}

int
MaxNodesFromGenerationParams(GenerateNodeGraph_Params params)
{
    return params.main_path_max + (params.main_path_max * params.max_linear_branch_length) + params.loop_generation_count;
}

int
MaxEdgesFromGenerationParams(GenerateNodeGraph_Params params)
{
    return (params.main_path_max - 1) + (params.main_path_max * params.max_linear_branch_length) + (2 * params.loop_generation_count);
}

void
GenerateNodeGraph(NodeGraph *graph, GenerateNodeGraph_Params params)
{
    if(*params.restart_count >= 100) return;
    if(!ValidArray(graph->nodes) or !ValidArray(graph->edges)) return;

    Assert(graph->nodes.max_count <= MaxNodesFromGenerationParams(params));
    Assert(graph->edges.max_count <= MaxEdgesFromGenerationParams(params));

    ClearArray(&graph->nodes);
    ClearArray(&graph->edges);

    // Main path
    u32 main_path_node_count = RandomU32(params.main_path_min, params.main_path_max);
    //ResizeArray(&graph->nodes, main_path_node_count);
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
        Edge edge = {.a=i-1, .b=i};
        //edge.rest_length = c::fdg_edge_length + RandomFloat(-30.f,30.f);
        if(!EdgeAlreadyExists(graph, edge)) graph->edges += edge;
    }

    // Linear branches

    int node_offset = 0;
    int cur_branch = 0;
    int max_branch = params.max_linear_branch_length;
    for(int i=0; i<main_path_node_count; ++i)
    {
        u32 branch_length = RandomU32(0, max_branch);
        for(int j=0; j<branch_length; ++j)
        {
            ++node_offset;

            Node node = {
                .pos = {410.f + 1.f*node_offset, 460.f+1.f*node_offset},
            };
            graph->nodes += node;

            Edge edge = {graph->nodes.count-1, i};
            Assert(!EdgeAlreadyExists(graph, edge))
            graph->edges += edge;
        }
    }

    int start_count = graph->nodes.count;
    for(int i=0; i<params.loop_generation_count; ++i)
    {
        ++node_offset;

        Node node = {
            .pos = {410.f + 1.f*node_offset, 460.f+1.f*node_offset},
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

//1137770496
//1139408896

// Returns true if graph is still in valid configuration after simulation step.
// Returns false otherwise.
ForceSimState
StepNodeGraphForceSimulation(NodeGraph *graph, ForceSimParams params, float dt, int iter_count)
{
    ForceSimState sim_state = {};

    for(int _=0; _<iter_count; ++_)
    {
        // Spring force for each edge
        for(int i=0; i<graph->edges.count; ++i)
        {
            Edge &edge = graph->edges[i];
            Node &node_a = graph->nodes[edge.a];
            Node &node_b = graph->nodes[edge.b];

            float cur_edge_length = Distance(node_a.pos, node_b.pos);
            Vec2f pos_a_to_b = node_b.pos - node_a.pos;
            Vec2f dv_a_to_b = dt * params.spring_constant * (cur_edge_length - params.edge_free_length) * pos_a_to_b;

            node_a.vel += dv_a_to_b;
            node_b.vel += -dv_a_to_b;
        }

        // Charge force between each pair of nodes
        for(int i=0; i<graph->nodes.count; ++i)
        {
            Node &node_a = graph->nodes[i];

            for(int j=0; j<graph->nodes.count; ++j)
            {
                if(j == i) continue; // no self-force

                Node &node_b = graph->nodes[j];

                float distance = Distance(node_a.pos, node_b.pos);
                float inv_dist_squared = 1.f / (distance * distance * distance);
                Vec2f pos_a_to_b = node_b.pos - node_a.pos;
                Vec2f dv_a_to_b = -dt*params.charge_strength * inv_dist_squared * pos_a_to_b;

                node_a.vel += dv_a_to_b;
                node_b.vel += -dv_a_to_b;
            }
        }

        // Apply friction and step position.
        for(Node &node : graph->nodes)
        {
            node.vel *= params.friction;
            node.pos += node.vel;
        }
    }

    for(int i=0; i<graph->edges.count; ++i)
    {
        int a0_index = graph->edges[i].a;
        int a1_index = graph->edges[i].b;
        Vec2f a0 = graph->nodes[a0_index].pos;
        Vec2f a1 = graph->nodes[a1_index].pos;

        for(int j=i+1; j<graph->edges.count; ++j)
        {
            int b0_index = graph->edges[j].a;
            int b1_index = graph->edges[j].b;
            Vec2f b0 = graph->nodes[b0_index].pos;
            Vec2f b1 = graph->nodes[b1_index].pos;

            if(LineSegmentsInnerIntersect(a0, a1, b0, b1, 0.01f))
            {
                //line_color = c::red;
                int temp = graph->edges[i].a;
                graph->edges[i].a = graph->edges[j].a;
                graph->edges[j].a = temp;

                if(!GraphIsFullyConnected(graph, params.temp_arena))
                {
                    sim_state.invalid_state = true;
                    sim_state.invalidation_reason = ForceSimInvalidationReason::NotFullyConnected;
                }

                break;
            }
        }
    }

    // Check for "bad" graph states (node speeds too high)
    float speed_sum = 0.f;
    float max_speed = 0.f;
    for(Node &node : graph->nodes)
    {
        max_speed = m::Max(max_speed, Length(node.vel));
        speed_sum += Length(node.vel);
    }

    sim_state.max_speed = max_speed;

    if(max_speed >= params.max_speed)
    {
        sim_state.invalid_state = true;
        sim_state.invalidation_reason = ForceSimInvalidationReason::MaxSpeedExceeded;
    }

    float average_speed = speed_sum / graph->nodes.count;
    if(average_speed < 0.001f) // TEMPORARILY CHANGED
    {
        sim_state.finished = true;
    }

    return sim_state;
}

NodeGraphResponse
TransformNodeGraphPointsToFitInsideRect(NodeGraph *graph, Rect rect)
{
    NodeGraphResponse response = {};

    Vec2f min = graph->nodes[0].pos;
    Vec2f max = min;

    for(auto node : graph->nodes)
    {
        if(node.pos.x < min.x) min.x = node.pos.x;
        if(node.pos.y < min.y) min.y = node.pos.y;
        if(node.pos.x > max.x) max.x = node.pos.x;
        if(node.pos.y > max.y) max.y = node.pos.y;
    }

    float x_range = max.x - min.x;
    float y_range = max.y - min.y;

    // Rect padded_rect = {};
    // padded_rect.pos = rect.pos + padding;
    // padded_rect.size = rect.size - 2*padding;

    //DrawUnfilledRect(padded_rect, c::grey);

    //Array<Node> transformed_nodes = CreateTempArray<Node>(graph->nodes.count);
    for(int i=0; i<graph->nodes.count; ++i)
    {
        auto &node = graph->nodes[i];
        if(x_range > y_range)
        {
            node.pos.x =   (rect.size.x / (max.x-min.x))*(node.pos.x - min.x) + rect.pos.x;
            node.pos.y =   (rect.size.y / (max.x-min.x))*(node.pos.y - min.y) + rect.pos.y
                         + (0.5f*rect.size.y - 0.5f*(rect.size.x / (max.x-min.x))*(max.y-min.y));
        }
        else
        {
            node.pos.x =   (rect.size.x / (max.y-min.y))*(node.pos.x - min.x) + rect.pos.x
                         + (0.5f*rect.size.x - 0.5f*(rect.size.x / (max.y-min.y))*(max.x-min.x));

            node.pos.y =   (rect.size.y / (max.y-min.y))*(node.pos.y - min.y) + rect.pos.y;
        }

        if(i == graph->start_index)
        {
            response.start_node_pos = node.pos;
        }
    }

    // for(auto edge : graph->edges)
    // {
    //     Vec2f a = transformed_nodes[edge.a].pos;
    //     Vec2f b = transformed_nodes[edge.b].pos;
    //     DrawLine(a, b, c::white);
    // }

    // for(int i=0; i<transformed_nodes.count; ++i)
    // {
    //     auto &node = transformed_nodes[i];
    //     Rect aligned_rect = AlignRect({node.pos, {10.f,10.f}}, c::align_center);

    //     if(i == graph->start_index)
    //     { // Start node
    //         response.start_node_pos = RectCenter(aligned_rect);
    //         DrawFilledRect(aligned_rect, c::green);
    //     }
    //     else if(i == graph->end_index)
    //     {
    //         DrawFilledRect(aligned_rect, c::yellow);
    //     }
    //     else
    //     {
    //         DrawFilledRect  (aligned_rect, c::black);
    //     }

    //     Color outline_color = c::red;
    //     if(MouseInRect(aligned_rect))
    //     {
    //         outline_color = c::white;
    //     }

    //     DrawUnfilledRect(aligned_rect, outline_color);
    // }

    return response;
}

NodeGraphResponse
DrawNodeGraph(NodeGraph *graph, float t)
{
    NodeGraphResponse response = {};

    for(auto edge : graph->edges)
    {
        Vec2f a = graph->nodes[edge.a].pos;
        Vec2f b = graph->nodes[edge.b].pos;
        DrawLine(a, b, c::lt_grey);
    }

    for(int i=0; i<graph->nodes.count; ++i)
    {
        auto &node = graph->nodes[i];

        Rect collision_rect = AlignRect({node.pos, {10.f,10.f}}, c::align_center);
        if(PointInRect(collision_rect, MousePos()) and !PointInRect(collision_rect, PrevMousePos()))
        {
            response.newly_hovered = true;
        }

        float t_for_this_node = 0.f;
        if(node.reachable and !node.completed) t_for_this_node = t;
        Rect aligned_rect = RectLerp( collision_rect,
                                      AlignRect({node.pos, {13.f,13.f}}, c::align_center),
                                      t_for_this_node);

        Color outline_color = c::red;
        if(MouseInRect(aligned_rect))
        {
            response.hovered_node_index = i;

            if(node.completed) outline_color = c::lt_grey; // Hovered and completed
            else               outline_color = c::white;   // Hovered and NOT completed
        }
        else
        {
            if(node.completed) outline_color = c::grey; // Completed (not hovered)
            else
            { // NOT completed (not hovered)
                if(node.reachable) outline_color = c::red;
                else               outline_color = c::lt_grey;
            }
        }

        if(i == graph->start_index)
        {
            response.start_node_pos = node.pos;
        }


        Color fill_color = c::black;
        if(i == graph->end_index) fill_color = c::green;
        else if(node.completed)   fill_color = c::black;
        else if(node.reachable)   fill_color = c::dk_red;
        else                      fill_color = c::grey;
        // if(i == response.hovered_node_index)
        // {
        //     if(node.completed)      fill_color = c::dk_grey;
        //     else if(node.reachable) fill_color = c::red;
        //     else                    fill_color = c::lt_grey;
        // }
        // else if(i == graph->start_index)
        // { // Start node
        //     fill_color = c::green;
        // }
        // else if(i == graph->end_index)
        // { // End node
        //     fill_color = c::yellow;
        // }
        // else
        // {
        //     fill_color = c::black;
        // }

        DrawFilledRect(  aligned_rect, fill_color);
        DrawUnfilledRect(aligned_rect, outline_color);
    }

    return response;
}

// @note: This doesn't work if the rect isn't a square.
NodeGraphResponse
DrawNodeGraphInRect(NodeGraph *graph, Rect rect)
{
    SetDrawDepth(c::def_draw_depth);
    NodeGraphResponse response = {};

    Vec2f min = graph->nodes[0].pos;
    Vec2f max = min;

    for(auto node : graph->nodes)
    {
        if(node.pos.x < min.x) min.x = node.pos.x;
        if(node.pos.y < min.y) min.y = node.pos.y;
        if(node.pos.x > max.x) max.x = node.pos.x;
        if(node.pos.y > max.y) max.y = node.pos.y;
    }

    float x_range = max.x - min.x;
    float y_range = max.y - min.y;

    //DrawUnfilledRect(padded_rect, c::grey);

    Array<Node> transformed_nodes = CreateTempArray<Node>(graph->nodes.count);
    for(auto node : graph->nodes)
    {
        Node transformed_node = {};
        if(x_range > y_range)
        {
            transformed_node.pos.x = (rect.size.x / (max.x-min.x))*(node.pos.x - min.x) + rect.pos.x;
            transformed_node.pos.y =   (rect.size.y / (max.x-min.x))*(node.pos.y - min.y) + rect.pos.y
                                     + (0.5f*rect.size.y - 0.5f*(rect.size.x / (max.x-min.x))*(max.y-min.y));
        }
        else
        {
            transformed_node.pos.x =   (rect.size.x / (max.y-min.y))*(node.pos.x - min.x) + rect.pos.x
                                     + (0.5f*rect.size.x - 0.5f*(rect.size.x / (max.y-min.y))*(max.x-min.x));

            transformed_node.pos.y =   (rect.size.y / (max.y-min.y))*(node.pos.y - min.y) + rect.pos.y;
        }

        transformed_nodes += transformed_node;
    }

    for(auto edge : graph->edges)
    {
        Vec2f a = transformed_nodes[edge.a].pos;
        Vec2f b = transformed_nodes[edge.b].pos;
        DrawLine(a, b, c::lt_grey);
    }

    for(int i=0; i<transformed_nodes.count; ++i)
    {
        auto &node = transformed_nodes[i];
        Rect aligned_rect = AlignRect({node.pos, {10.f,10.f}}, c::align_center);

        if(i == graph->start_index)
        { // Start node
            response.start_node_pos = RectCenter(aligned_rect);
            DrawFilledRect(aligned_rect, c::green);
        }
        else if(i == graph->end_index)
        {
            DrawFilledRect(aligned_rect, c::yellow);
        }
        else
        {
            DrawFilledRect  (aligned_rect, c::black);
        }

        Color outline_color = c::red;
        if(MouseInRect(aligned_rect))
        {
            response.hovered_node_index = i;
            outline_color = c::white;
        }

        DrawUnfilledRect(aligned_rect, outline_color);
    }

    return response;
}