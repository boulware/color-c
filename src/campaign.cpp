#include "campaign.h"

#include "random.h"
#include "generate_node_graph_params.h"

// bool
// GraphIsFullyConnected(Campaign *campaign)
// {
//     //Node &root =  campaign->nodes[0];

//     Array<int> indices_to_search = CreateTempArray<int>(20);
//     indices_to_search += 0; // add "root"

//     Array<int> searched_indices = CreateTempArray<int>(20);
//     Array<Edge> connected_edges = CreateTempArray<Edge>(10);
//     //for(int i=0; i<campaign->edges.count; ++i)
//     while(indices_to_search.count > 0)
//     {
//         ClearArray(&connected_edges);
//         int this_index = indices_to_search[indices_to_search.count-1];
//         indices_to_search.count -= 1;
//         searched_indices += this_index;
//         for(auto edge : campaign->edges)
//         {
//             if(this_index == edge.indices[0])
//             {
//                 //connected_edges += edge;
//                 if(     !ElementInArray(&searched_indices, edge.indices[1])
//                     and !ElementInArray(&indices_to_search, edge.indices[1]))
//                 {
//                     indices_to_search += edge.indices[1];
//                 }
//             }
//             else if(this_index == edge.indices[1])
//             {
//                 //connected_edges += edge;
//                 if(    !ElementInArray(&searched_indices, edge.indices[0])
//                    and !ElementInArray(&indices_to_search, edge.indices[0]))
//                 {
//                     indices_to_search += edge.indices[0];
//                 }
//             }
//         }
//     }

//     campaign->graph_fully_connected = (searched_indices.count == campaign->nodes.count);
//     return campaign->graph_fully_connected;
// }

// Node *
// AddNode(Campaign *campaign, Node node, Array<int> edge_indices)
// {
//     int new_node_index = campaign->nodes.count;
//     Node *new_node = Append(&campaign->nodes, node);

//     if(edge_indices.data)
//     {
//         for(int existing_node_index : edge_indices)
//         {
//             Edge edge;
//             edge.indices[0] = existing_node_index;
//             edge.indices[1] = new_node_index;
//             campaign->edges += edge;
//         }
//     }

//     return new_node;
// }

// void
// GenerateRandomTree(Campaign *campaign)
// {
//     TIMED_BLOCK;

//     ClearArray(&campaign->nodes);
//     ClearArray(&campaign->edges);
//     campaign->drag_start_index = -1;

//     u32 node_count = RandomU32(8, 16);
//     ResizeArray(&campaign->nodes, node_count);

//     for(int i=0; i<node_count; ++i)
//     {

//         u32 node_x = RandomU32(300, ClampFloatToU32(game->window_size.x)-300);
//         u32 node_y = RandomU32(300, ClampFloatToU32(game->window_size.y)-300);

//         Node node = {
//             .pos = {(float)node_x, (float)node_y},
//             .vel = {0.f,0.f}
//         };

//         campaign->nodes += node;
//     }

//     Array<Edge> edge_permutations = CreateTempArray<Edge>(100);
//     for(int i=0; i<node_count; ++i)
//     {
//         for(int j=i; j<node_count; ++j)
//         {
//             Edge edge;
//             edge.indices[0] = i;
//             edge.indices[1] = j;
//             edge_permutations += edge;
//         }
//     }

//     int max_edge_count = 3;
//     Array<int> edges_per_node = CreateTempArray<int>(node_count);
//     for(int i=0; i<node_count; ++i) edges_per_node += 0;

//     for(auto edge : edge_permutations)
//     {
//         int p = RandomU32(0,5);
//         int a_index = edge.indices[0];
//         int b_index = edge.indices[1];
//         if(p == 0 and edges_per_node[a_index] < max_edge_count and edges_per_node[b_index] < max_edge_count)
//         {
//             campaign->edges += edge;
//             ++edges_per_node[a_index];
//             ++edges_per_node[b_index];
//         }
//     }

//     int nodes_without_edges = 0;
//     for(int v : edges_per_node)
//     {
//         if(v == 0) ++nodes_without_edges;
//     }

//     if(nodes_without_edges > node_count/4)
//     {
//         GenerateRandomTree(campaign);
//     }
// }

// void
// GenerateTreeFromMainBranch(Campaign *campaign)
// {
//     ++campaign->generation_count;

//     ClearArray(&campaign->nodes);
//     ClearArray(&campaign->edges);
//     campaign->drag_start_index = -1;

//     int m = 2;

//     // Main path
//     u32 main_path_node_count = RandomU32(m*6,m*8);
//     ResizeArray(&campaign->nodes, main_path_node_count);
//     for(int i=0; i<main_path_node_count; ++i)
//     {
//         Node node = {
//             .pos = {400.f + 10.f*i, 450.f},
//             .vel = {0.f,0.f}//{(float)RandomU32(0,2)-1, (float)RandomU32(0,2)-1}
//         };
//         campaign->nodes += node;
//     }

//     campaign->end_index = campaign->nodes.count-1;

//     for(int i=1; i<campaign->nodes.count; ++i)
//     {
//         Edge edge = {};
//         edge.indices[0] = i-1;
//         edge.indices[1] = i;
//         edge.rest_length = c::fdg_edge_length + RandomFloat(-30.f,30.f);
//         if(!EdgeAlreadyExists(campaign, edge)) campaign->edges += edge;
//     }

//     // Linear branches

//     int j = 0;
//     int cur_branch = 0;
//     int max_branch = 3;
//     for(int i=0; i<main_path_node_count; ++i)
//     {
//         ++j;
//         if(Chance(150.0f/m))
//         {
//             Node node = {
//                 .pos = {410.f + 1.f*j, 460.f+1.f*j},
//                 .vel = {0.f,0.f}//{(float)RandomU32(0,2)-1, (float)RandomU32(0,2)-1}
//             };
//             campaign->nodes += node;

//             //int attach_index = RandomU32(1, main_path_node_count-2);
//             Edge edge = {};
//             edge.indices[0] = campaign->nodes.count-1;
//             edge.indices[1] = i;//attach_index;
//             edge.rest_length = c::fdg_edge_length + RandomFloat(-30.f,30.f);
//             if(!EdgeAlreadyExists(campaign, edge)) campaign->edges += edge;

//             if(++cur_branch >= max_branch) break;
//             --i;
//         }
//     }

//     int start_count = campaign->nodes.count;
//     for(int i=0; i<m*5; ++i)
//     {
//         ++j;
//         //if(Chance(50.0f))
//         {
//             Node node = {
//                 .pos = {410.f + 1.f*j, 460.f+1.f*j},
//                 .vel = {0.f,0.f}//{(float)RandomU32(0,2)-1, (float)RandomU32(0,2)-1}
//             };
//             campaign->nodes += node;

//             Edge edge = {};
//             edge.indices[0] = campaign->nodes.count-1;
//             edge.indices[1] = RandomU32(0, campaign->nodes.count-2);;//attach_index;
//             if(!EdgeAlreadyExists(campaign, edge)) campaign->edges += edge;

//             edge.indices[0] = campaign->nodes.count-1;
//             edge.indices[1] = RandomU32(0, campaign->nodes.count-2);;//attach_index;
//             if(!EdgeAlreadyExists(campaign, edge)) campaign->edges += edge;
//         }
//     }
// }

void
InitCampaign(Campaign *campaign)
{
    // campaign->map_generation_temp_arena = AllocArena();
    // campaign->map_generation_arena = AllocArena();

    campaign->arena = AllocArena();

    campaign->generation_params_template = {
        .thread_finished = nullptr,
        .restart_count   = nullptr,
        .max_speed       = nullptr,
        .graph           = nullptr,
        .main_path_min = 12,
        .main_path_max = 16,
        .max_linear_branch_length = 3,
        .linear_branch_extension_chance = 50.0f,
        .loop_generation_count = 10,
    };

    int max_nodes = MaxNodesFromGenerationParams(campaign->generation_params_template);
    int max_edges = MaxEdgesFromGenerationParams(campaign->generation_params_template);
    for(int i=0; i<ArrayCount(campaign->maps); ++i)
    {
        campaign->maps[i].nodes = CreateArrayFromArena<Node>(max_nodes, &campaign->arena);
        campaign->maps[i].edges = CreateArrayFromArena<Edge>(max_edges, &campaign->arena);
    }

    platform->CreateWorkQueue(&campaign->map_generation_work_queue, 3, "Map generation");

    for(int i=0; i<ArrayCount(campaign->maps); ++i)
    {
        campaign->max_speeds[i] = 100.f;

        GenerateNodeGraph_Params params = campaign->generation_params_template;
        params.thread_finished = &campaign->generation_finished[i];
        params.restart_count   = &campaign->restart_counts[i];
        params.max_speed       = &campaign->max_speeds[i];
        params.graph           = &campaign->maps[i];

        WorkEntry entry = {
            .callback = THREAD_GenerateNodeGraph,
            .data = &params,
            .data_byte_count = sizeof(params)
        };

        platform->AddWorkEntry(campaign->map_generation_work_queue, entry);
    }

    // Log("Thread %d started.", platform->StartJob(&params));
    // Log("Thread %d started.", platform->StartJob(&params));
    // Log("Thread %d started.", platform->StartJob(&params));
    // Log("Thread %d started.", platform->StartJob(&params));
    // Log("Thread %d started.", platform->StartJob(&params));
    // Log("Thread %d started.", platform->StartJob(&params));
    // Log("Thread %d started.", platform->StartJob(&params));
    // Log("Thread %d started.", platform->StartJob(&params));
    // GenerateNodeGraph(&campaign->map, params);
    // campaign->nodes = CreatePermanentArray<Node>(10);
    // campaign->edges = CreatePermanentArray<Edge>(10);

    // Node root_node = {
    //     .pos = 0.5f * game->window_size
    // };
    // campaign->root = AddNode(campaign, root_node);

    // campaign->generation_timer = {
    //     .start = 0.f,
    //     .cur = 0.f,
    //     .length_s = 0.8f,
    //     .finished = false
    // };
    // campaign->generation_finished = false;
    // campaign->drag_start_index = -1;
    // campaign->fdg_running = true;
}

GameState
TickCampaign(Campaign *campaign)
{
    // if(campaign->generation_finished)
    // {
    //     GenerateNodeGraph_Params params = {
    //         .graph = &campaign->map,
    //         .campaign = campaign,
    //         .main_path_min = 12,
    //         .main_path_max = 16,
    //         .max_linear_branch_length = 3,
    //         .linear_branch_extension_chance = 50.0f,
    //         .loop_generation_count = 10,
    //     };

    //     platform->StartJob(&params);
    //     campaign->generation_finished = false;
    // }

    bool map_generation_finished = false;
    if(platform->WorkQueuePendingJobCount(campaign->map_generation_work_queue) == 0)
    {
        map_generation_finished = true;
    }

    if(Pressed(vk::enter) and map_generation_finished)
    {
        for(int i=0; i<ArrayCount(campaign->maps); ++i)
        {
            // Don't restart generation if the first one hasn't finished.
            if(campaign->generation_finished[i] == false) continue;

            campaign->generation_finished[i] = false;
            campaign->restart_counts[i] = 0;
            campaign->max_speeds[i] = 100.f;

            GenerateNodeGraph_Params params = campaign->generation_params_template;
            params.thread_finished = &campaign->generation_finished[i];
            params.restart_count   = &campaign->restart_counts[i];
            params.max_speed       = &campaign->max_speeds[i];
            params.graph           = &campaign->maps[i];

            WorkEntry entry = {
                .callback = THREAD_GenerateNodeGraph,
                .data = &params,
                .data_byte_count = sizeof(params)
            };

            platform->AddWorkEntry(campaign->map_generation_work_queue, entry);
        }
    }

    for(int i=0; i<ArrayCount(campaign->maps); ++i)
    {
        Rect map_rect = AlignRect({{(1.f/6.f)*(1+2*i)*game->window_size.x, 0.5f*game->window_size.y},
                                   {400.f,400.f}}, c::align_center);
        //if(campaign->generation_finished[i])
        if(campaign->max_speeds[i] < 0.1f)
        {
            //DrawNodeGraphInRect(&campaign->map, {{50.f,50.f},{600.f,200.f}});
            DrawNodeGraphInRect(&campaign->maps[i], map_rect, {10.f,10.f});
        }
        else
        {
            TextLayout layout = c::def_text_layout;
            layout.align = c::align_center;
            DrawText(layout, RectCenter(map_rect), "Generating Map... ");
        }

        //DrawText(c::def_text_layout, RectTopLeft(map_rect), "max speed: %f", campaign->max_speeds[i]);
        DrawUnfilledRect(map_rect, c::white);

        if(map_generation_finished)
        {
            Rect button_rect = {};
            Vec2f padding = {0.f, 10.f};
            button_rect.pos = RectBottomCenter(map_rect) + padding;
            button_rect.size = {200.f, 80.f};
            ButtonLayout button_layout = c::def_button_layout;
            button_layout.align = c::align_topcenter;
            auto response = DrawButton(button_layout, button_rect, "Use this map");
        }
    }


    //DrawTextMultiline(c::small_text_layout, MousePos(), MetaString(campaign));

    // float cam_move_speed = 20.f;
    // if(Down(vk::right))
    // {
    //     campaign->camera_offset.x -= cam_move_speed;
    //     //SetCameraPos(game->camera_pos);
    // }
    // if(Down(vk::left))
    // {
    //     campaign->camera_offset.x += cam_move_speed;
    //     //SetCameraPos(game->camera_pos);
    // }
    // if(Down(vk::up))
    // {
    //     campaign->camera_offset.y += cam_move_speed;
    //     //SetCameraPos(game->camera_pos);
    // }
    // if(Down(vk::down))
    // {
    //     campaign->camera_offset.y -= cam_move_speed;
    //     //SetCameraPos(game->camera_pos);
    // }

    // if(Pressed(vk::m)) game->camera_pos = 0.5f*game->window_size;

    // if(Pressed(vk::enter))
    // //if(Tick(&campaign->generation_timer))
    // {
    //     campaign->max_speed = 0.f;
    //     GenerateTreeFromMainBranch(campaign);
    //     //SetCameraPos({0.f,0.f});
    //     campaign->generation_finished = false;
    //     Reset(&campaign->generation_timer);
    // }

    // if(Pressed(vk::space))
    // {
    //     campaign->fdg_running = !campaign->fdg_running;
    // }

    // if(Pressed(vk::RMB))
    // {
    //     // AddNode(Campaign *campaign, Node node, Array<int> edge_indices)
    //     Node node = {
    //         .pos = MousePos()
    //     };
    //     AddNode(campaign, node);
    // }






    // Color line_color = c::white;
    // if(campaign->generation_finished) line_color = c::white;

    // for(int i=0; i<campaign->edges.count; ++i)
    // {
    //     int a0_index = campaign->edges[i].indices[0];
    //     int a1_index = campaign->edges[i].indices[1];
    //     Vec2f a0 = campaign->nodes[a0_index].pos;
    //     Vec2f a1 = campaign->nodes[a1_index].pos;

    //     for(int j=i+1; j<campaign->edges.count; ++j)
    //     {
    //         int b0_index = campaign->edges[j].indices[0];
    //         int b1_index = campaign->edges[j].indices[1];
    //         Vec2f b0 = campaign->nodes[b0_index].pos;
    //         Vec2f b1 = campaign->nodes[b1_index].pos;

    //         if(LineSegmentsInnerIntersect(a0, a1, b0, b1, 0.01f))
    //         {
    //             //line_color = c::red;
    //             int temp = campaign->edges[i].indices[0];
    //             campaign->edges[i].indices[0] = campaign->edges[j].indices[0];
    //             campaign->edges[j].indices[0] = temp;

    //             if(!GraphIsFullyConnected(campaign)) GenerateTreeFromMainBranch(campaign);

    //             break;
    //         }
    //     }

    //     DrawLine(a0, a1, line_color);
    // }

    // DrawText(c::def_text_layout, {0.f,0.f}, "generations: %d", campaign->generation_count);


    // for(int i=0; i<campaign->nodes.count; ++i)
    // {
    //     Node &node = campaign->nodes[i]; // alias

    //     Rect aligned_rect = AlignRect({node.pos, c::node_size}, c::align_center);

    //     DrawFilledRect(aligned_rect, c::black);

    //     Color outline_color = c::red;

    //     if(PointInRect(aligned_rect, MousePos() - campaign->camera_offset))
    //     {
    //         outline_color = c::white;
    //     }
    //     else if(i == 0)                      outline_color = c::green;
    //     else if(i == campaign->end_index)    outline_color = c::yellow;

    //     DrawUnfilledRect(aligned_rect, outline_color);
    // }

    // //DrawText(c::def_text_layout, {0.f,0.f}, "%d", campaign->edges.count);

    // if(Released(vk::LMB))
    // {
    //     campaign->drag_start_index = -1;
    // }

    GameState new_state = GameState::None;
    if(Pressed(KeyBind::Exit)) new_state = GameState::MainMenu;

    return new_state;
}