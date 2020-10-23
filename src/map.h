#ifndef MAP_H
#define MAP_H

struct MapResponse
{
    Vec2f start_node_pos;
    int hovered_node_index = -1;
    bool newly_hovered;
};

MapResponse DrawMap(NodeGraph graph, Array<Room> rooms, float node_pulse_t);

#endif