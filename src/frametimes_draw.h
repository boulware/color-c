#ifndef FRAMETIMES_DRAW_H
#define FRAMETIMES_DRAW_H

struct FrametimeGraphState
{
    float *frametimes;
    int cur_frametime_index;
    int entry_count;

    float graph_max;
    ImguiContainer ui_container;
    TextLayout label_text;
};

#endif