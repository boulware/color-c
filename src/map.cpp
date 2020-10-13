#include "map.h"
#include "node_graph.h"

MapResponse
DrawMap(NodeGraph graph, Array<Room> rooms, float node_pulse_t)
{
    MapResponse response = {};

    for(auto edge : graph.edges)
    {
        Vec2f a = graph.nodes[edge.a].pos;
        Vec2f b = graph.nodes[edge.b].pos;
        DrawLine(a, b, c::lt_grey);
    }

    for(int i=0; i<graph.nodes.count; ++i)
    {
        auto &node = graph.nodes[i];

        Rect collision_rect = AlignRect({node.pos, {10.f,10.f}}, c::align_center);
        if(PointInRect(collision_rect, MousePos()) and !PointInRect(collision_rect, PrevMousePos()))
        {
            response.newly_hovered = true;
        }

        float t_for_this_node = 0.f;
        if(node.reachable and !node.completed) t_for_this_node = node_pulse_t;
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

        if(i == graph.start_index)
        {
            response.start_node_pos = node.pos;
        }


        Color fill_color = c::black;
        if(i == graph.end_index) fill_color = c::green;
        else if(node.completed)   fill_color = c::black;
        else if(node.reachable)   fill_color = c::dk_red;
        else                      fill_color = c::grey;
        // if(i == response.hovered_node_index)
        // {
        //     if(node.completed)      fill_color = c::dk_grey;
        //     else if(node.reachable) fill_color = c::red;
        //     else                    fill_color = c::lt_grey;
        // }
        // else if(i == graph.start_index)
        // { // Start node
        //     fill_color = c::green;
        // }
        // else if(i == graph.end_index)
        // { // End node
        //     fill_color = c::yellow;
        // }
        // else
        // {
        //     fill_color = c::black;
        // }

        DrawFilledRect(  aligned_rect, fill_color);
        DrawUnfilledRect(aligned_rect, outline_color);

        TextLayout text_layout = c::def_text_layout;
        text_layout.align = c::align_center;
        text_layout.font_size = 4;
        DrawText(text_layout, RectCenter(aligned_rect), RoomTypeUserString(rooms[i].type));
    }

    return response;
}