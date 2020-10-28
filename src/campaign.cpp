#include "campaign.h"

#include "random.h"
#include "generate_node_graph_params.h"
#include "camera.h"
#include "map.h"

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

    campaign->state = CampaignState::MapSelection;

    campaign->arena_id = AllocArena("Campaign");
    campaign->battle_arena_id = AllocArena("Campaign Battle");

    // Maps
    int max_nodes = MaxNodesFromGenerationParams(campaign->generation_params_template);
    int max_edges = MaxEdgesFromGenerationParams(campaign->generation_params_template);
    for(int i=0; i<ArrayCount(campaign->maps); ++i)
    {
        campaign->maps[i].nodes = CreateArrayFromArena<Node>(max_nodes, campaign->arena_id);
        campaign->maps[i].edges = CreateArrayFromArena<Edge>(max_edges, campaign->arena_id);
    }
    campaign->selected_map_index = -1;

    // Rooms
    campaign->rooms = CreateArrayFromArena<Room>(max_nodes, campaign->arena_id);
    campaign->room_is_init = false;

    campaign->map_zoom_timer = {
        .start = 0.f,
        .cur = 0.f,
        .length_s = 0.1f,//2.5f,
        .finished = false,
    };

    campaign->node_pulse_timer = {
        .low = 0.f,
        .high = 1.f,
    };

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

    Rect rect = {
        .pos  = Vec2f{0.f,0.f},
        .size = Vec2f{1600.f, 900.f}
    };
    MoveCameraToWorldRect(&game->camera, rect);

    campaign->player_party = CreateArrayFromArena<UnitId>(c::max_party_size, campaign->arena_id);
    campaign->player_party += CreateUnitByName("Warrior", Team::allies, campaign->arena_id);
    //campaign->player_party += CreateUnitByName("Warrior", Team::allies, campaign->arena_id);
    //AddUnitToUnitSet(CreateUnitByName("Warrior", Team::allies), &campaign->player_party);

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
    //for(int i=0; i<1; ++i)
    // if(RandomU32(0, 2) == 0)
    //     Log("Tick");

    if(campaign->state == CampaignState::MapSelection)
    {
        // Check map generation status.
        bool map_generation_finished = false;
        if(platform->WorkQueuePendingJobCount(campaign->map_generation_work_queue) == 0)
        {
            map_generation_finished = true;
        }

        // Redo map generation when Enter is pressed.
        if(Pressed(vk::enter) and map_generation_finished)
        {
            for(int i=0; i<ArrayCount(campaign->maps); ++i)
            {
                campaign->show_generation[i] = false;
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

                //for(int j=0; j<10; ++j)
                platform->AddWorkEntry(campaign->map_generation_work_queue, entry);
                //for(int j=0; j<1000; ++j)
            }
        }

        // Draw maps and "Use this map" button.
        for(int i=0; i<ArrayCount(campaign->maps); ++i)
        {
            Rect map_rect = AlignRect({{(1.f/6.f)*(1+2*i)*game->window_size.x, 0.5f*game->window_size.y},
                                       {400.f,400.f}}, c::align_center);
            //if(campaign->generation_finished[i])
            if(campaign->max_speeds[i] < 0.1f)
                campaign->show_generation[i] = true;

            if(campaign->show_generation[i])
            {
                DrawNodeGraphInRect(&campaign->maps[i], map_rect);
            }
            else
            {
                TextLayout layout = c::def_text_layout;
                layout.align = c::align_center;
                DrawText(layout, RectCenter(map_rect), "Generating Map... ");
            }

            Vec2f map_padding = {10.f,10.f};
            Rect padded_map_rect = {map_rect.pos - map_padding, map_rect.size + 2*map_padding};
            DrawUnfilledRect(padded_map_rect, c::white);

            if(map_generation_finished)
            {
                Rect button_rect = {};
                Vec2f padding = {0.f, 10.f};
                button_rect.pos = RectBottomCenter(map_rect) + padding;
                button_rect.size = {200.f, 80.f};
                ButtonLayout button_layout = c::def_button_layout;
                button_layout.align = c::align_topcenter;
                auto response = DrawButton(button_layout, button_rect, "Use this map");

                if(response.pressed)
                {
                    campaign->selected_map_index = i;
                    campaign->state = CampaignState::TransitionIntoMap;
                    Reset(&campaign->map_zoom_timer);

                    Rect world_space_rect = {{0.f,0.f}, {400.f,400.f}};
                    auto graph_response = TransformNodeGraphPointsToFitInsideRect(&campaign->maps[i], map_rect);

                    campaign->start_camera = game->camera;
                    // campaign->camera_start_pos = game->camera_pos;
                    // campaign->camera_start_rect = {game->camera_pos - 0.5f*game->camera_view, game->camera_view};
                    campaign->start_node_pos = graph_response.start_node_pos;
                    Vec2f center_of_screen_in_world_space = ScreenPointToWorldPoint(game->camera, 0.5f*game->window_size);
                    Vec2f point_lerp_vector = center_of_screen_in_world_space - campaign->start_node_pos;
                    Vec2f camera_lerp_vector = -point_lerp_vector;

                    campaign->end_camera.pos = campaign->start_camera.pos + camera_lerp_vector;
                    campaign->end_camera.view = map_rect.size/2.f;

                    //SetCameraPos(campaign->camera_start_pos);
                    //MoveCameraToWorldRect(world_space_rect);
                }
            }
        }
    }
    else if(campaign->state == CampaignState::TransitionIntoMap)
    {
        Tick(&campaign->map_zoom_timer);

        int i = campaign->selected_map_index;
        Rect initial_map_rect = AlignRect({{(1.f/6.f)*(1+2*i)*game->window_size.x, 0.5f*game->window_size.y},
                                           {400.f,400.f}}, c::align_center);
        Rect final_map_rect = {{}, {800.f,800.f}};
        // float t = campaign->map_zoom_timer.cur;
        // float T = campaign->map_zoom_timer.length_s;
        // float t_over_T = t / T;
        // float cubic_t = -2*m::Pow(t_over_T, 3) + 3*m::Pow(t_over_T, 2);
        // Rect lerped_rect = RectLerp(initial_map_rect, final_map_rect, cubic_t);

        float t = campaign->map_zoom_timer.cur;
        float T = campaign->map_zoom_timer.length_s;
        float t_over_T = t / T;
        float cubic_t = -2*m::Pow(t_over_T, 3) + 3*m::Pow(t_over_T, 2);

        //ZoomCameraIntoPoint(1.03f, campaign->start_node_pos);
        //SetCameraPos( Lerp(campaign->camera_start_pos, campaign->camera_end_pos, t_over_T) );
        Camera lerped_cam = LerpCamera(campaign->start_camera, campaign->end_camera, cubic_t);
        SetCameraPos(&game->camera, lerped_cam.pos);
        SetCameraView(&game->camera, PadToAspectRatio(lerped_cam.view, AspectRatio(game->window_size)));

        // MoveCameraToWorldRect(&game->camera,
        //                       Rect{    Lerp(campaign->camera_start_pos, campaign->camera_end_pos, 0.f),
        //                            RectLerp(campaign->camera_start_rect, campaign->camera_end_rect, 0.f).size} );
                                        //campaign->camera_start_rect.size});

        //Tick(&campaign->node_pulse_timer);
        DrawNodeGraph(&campaign->maps[i]);




        if(campaign->map_zoom_timer.finished)
        {
            campaign->state = CampaignState::InMap;

            NodeGraph &current_map = campaign->maps[campaign->selected_map_index];
            //for(auto &room : campaign->rooms)
            for(int _=0; _<current_map.nodes.count; ++_)
            {
                //Room room = {.type = (RoomType)RandomU32(2, (u32)RoomType::COUNT-1)};
                Room room = {.type = RoomType::Battle};
                campaign->rooms += room;
            }

            campaign->rooms[current_map.end_index].type = RoomType::Boss;
        }
    }
    else if(campaign->state == CampaignState::InMap)
    {
        SetDrawDepth(50.f);

        // Move around map with WASD
        float cam_movespeed = 10.f;
        if(Down(vk::S))
        {
            MoveCamera(&game->camera, {0.f, cam_movespeed});
        }
        if(Down(vk::W))
        {
            MoveCamera(&game->camera, {0.f, -cam_movespeed});
        }
        if(Down(vk::A))
        {
            MoveCamera(&game->camera, {-cam_movespeed, 0.f});
        }
        if(Down(vk::D))
        {
            MoveCamera(&game->camera, {cam_movespeed, 0.f});
        }

        // Mouse clicking and dragging moves around map
        if(Down(vk::RMB) and MouseMoved())
        {
            Vec2f mouse_move = RelativeMousePos();
            MoveCamera(&game->camera, -mouse_move);
        }

        // Mouse scroll zooms in and out
        int mouse_scroll = MouseScroll();
        if(mouse_scroll != 0)
        {
            float zoom_amount = m::Pow(c::zoom_sensitivity, -mouse_scroll);
            SetCameraView(&game->camera, zoom_amount * game->camera.view);
        }

        Rect map_rect = {{}, {100.f,100.f}};
        Tick(&campaign->node_pulse_timer);
        float cubic_t = ParameterToCubic(campaign->node_pulse_timer.cur, campaign->node_pulse_timer.high);
        auto map_response = DrawMap(campaign->maps[campaign->selected_map_index],
                                    campaign->rooms,
                                    cubic_t);

        if(Pressed(vk::LMB) and map_response.hovered_node_index >= 0)
        {
            Node &node = campaign->maps[campaign->selected_map_index].nodes[map_response.hovered_node_index]; // alias
            if(node.reachable)
            {
                CompleteNode(&campaign->maps[campaign->selected_map_index], map_response.hovered_node_index);
                campaign->state = CampaignState::InRoom;
                campaign->current_room_index = map_response.hovered_node_index;
            }
        }

        Camera initial_camera = game->camera;
        MoveCameraToWorldRect(&game->camera, {{0.f,0.f}, game->window_size});

        Rect side_panel_rect = {{}, {200.f, game->window_size.y}};
        DrawFilledRect(side_panel_rect, c::dk_grey);
        DrawLine(RectTopRight(side_panel_rect), RectBottomRight(side_panel_rect), c::white);
        Vec2f pen = {5.f, 0.f};

        for(auto unit_id : campaign->player_party)
        {
            Unit *unit = GetUnitFromId(unit_id);
            if(!unit) continue;

            Vec2f name_size = DrawText(c::def_text_layout, pen, unit->name).rect.size;
            DrawTraitSetWithPreview(pen + Vec2f{0.f, name_size.y},
                                    unit->cur_traits,
                                    unit->max_traits,
                                    unit->cur_traits,
                                    0.f);
            pen += {0.f, 100.f};
        }

        SetCameraPos(&game->camera, initial_camera.pos);
        SetCameraView(&game->camera, initial_camera.view);
        //if(map_response.newly_hovered) ResetLow(&campaign->node_pulse_timer);
    }
    else if(campaign->state == CampaignState::InRoom)
    {
        Room current_room = campaign->rooms[campaign->current_room_index];
        if(current_room.type == RoomType::Battle)
        {
            if(!campaign->room_is_init)
            {
                InitBattle(&campaign->current_battle, campaign->battle_arena_id);
                Array<UnitId> battle_units = CreateTempArray<UnitId>(2*c::max_party_size);
                AppendArrayToArray(&battle_units, campaign->player_party); // Add ally units
                int enemy_count = RandomU32(1,4);
                for(int i=0; i<enemy_count; ++i)
                { // Add enemy units
                    #if 1
                        battle_units += CreateUnit(RandomBreedIdByTier(1), Team::enemies, campaign->battle_arena_id);
                    #else
                        battle_units += CreateUnitByName("Test", Team::enemies, campaign->battle_arena_id);
                    #endif
                }

                StartBattle(&campaign->current_battle, battle_units);

                campaign->map_camera = game->camera;
                MoveCameraToWorldRect(&game->camera, {{0.f,0.f}, game->window_size});

                campaign->room_is_init = true;

            }

            auto battle_state = TickBattle(&campaign->current_battle);
            if(battle_state.finished)
            {
                SetCameraPos(&game->camera, campaign->map_camera.pos);
                SetCameraView(&game->camera, campaign->map_camera.view);
                for(auto unit_id : campaign->player_party)
                    FullHealUnit(unit_id);

                campaign->room_is_init = false;
                campaign->state = CampaignState::InMap;
            }
        }
    }




    GameState new_state = GameState::None;
    if(Pressed(KeyBind::Exit)) new_state = GameState::MainMenu;

    return new_state;
}