void
GenerateTreeFromMainBranch(Campaign *campaign)
{
    ++campaign->generation_count;

    ClearArray(&campaign->nodes);
    ClearArray(&campaign->edges);
    campaign->drag_start_index = -1;

    int m = 2;

    // Main path
    u32 main_path_node_count = RandomU32(m*6,m*8);
    ResizeArray(&campaign->nodes, main_path_node_count);
    for(int i=0; i<main_path_node_count; ++i)
    {
        Node node = {
            .pos = {400.f + 10.f*i, 450.f},
            .vel = {0.f,0.f}//{(float)RandomU32(0,2)-1, (float)RandomU32(0,2)-1}
        };
        campaign->nodes += node;
    }

    campaign->end_index = campaign->nodes.count-1;

    for(int i=1; i<campaign->nodes.count; ++i)
    {
        Edge edge = {};
        edge.indices[0] = i-1;
        edge.indices[1] = i;
        edge.rest_length = c::fdg_edge_length + RandomFloat(-30.f,30.f);
        if(!EdgeAlreadyExists(campaign, edge)) campaign->edges += edge;
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
            campaign->nodes += node;

            //int attach_index = RandomU32(1, main_path_node_count-2);
            Edge edge = {};
            edge.indices[0] = campaign->nodes.count-1;
            edge.indices[1] = i;//attach_index;
            edge.rest_length = c::fdg_edge_length + RandomFloat(-30.f,30.f);
            if(!EdgeAlreadyExists(campaign, edge)) campaign->edges += edge;

            if(++cur_branch >= max_branch) break;
            --i;
        }
    }

    int start_count = campaign->nodes.count;
    for(int i=0; i<m*5; ++i)
    {
        ++j;
        //if(Chance(50.0f))
        {
            Node node = {
                .pos = {410.f + 1.f*j, 460.f+1.f*j},
                .vel = {0.f,0.f}//{(float)RandomU32(0,2)-1, (float)RandomU32(0,2)-1}
            };
            campaign->nodes += node;

            Edge edge = {};
            edge.indices[0] = campaign->nodes.count-1;
            edge.indices[1] = RandomU32(0, campaign->nodes.count-2);;//attach_index;
            if(!EdgeAlreadyExists(campaign, edge)) campaign->edges += edge;

            edge.indices[0] = campaign->nodes.count-1;
            edge.indices[1] = RandomU32(0, campaign->nodes.count-2);;//attach_index;
            if(!EdgeAlreadyExists(campaign, edge)) campaign->edges += edge;
        }
    }
}