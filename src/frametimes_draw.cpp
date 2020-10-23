#include "frametimes_draw.h"

#include "ring_buffer.h"
#include "vec.h"

void
InitFrametimeGraphState(FrametimeGraphState *state, int entry_count)
{
    size_t byte_count = entry_count * sizeof(float);
    state->frametimes = (float *)AllocPerma(byte_count);
    ZeroMemoryBlock(state->frametimes, byte_count);
    state->cur_frametime_index = 0;
    state->entry_count = entry_count;

    state->graph_max = c::frametime_def_graph_max;
}

void
AddFrametimeToGraph(FrametimeGraphState *state, float frametime_value)
{
    state->frametimes[state->cur_frametime_index++] = frametime_value;
    if(state->cur_frametime_index == state->entry_count) state->cur_frametime_index = 0;
}

void
DrawFrametimes(FrametimeGraphState *state, Rect graph_rect)
{
    // DrawFilledRect(graph_rect, c::vdk_red, true);
    // DrawUnfilledRect(graph_rect, c::white, true);

    // float max_frametime = 0.f;
    // for(int i=0; i<frametime_count; ++i)
    // {
    //     max_frametime = m::Max(frametimes[i], max_frametime);
    // }

    float bar_width = graph_rect.size.x / state->entry_count;
    Vec2f origin = RectBottomLeft(graph_rect);
    for(int i=0; i<state->entry_count; ++i)
    {
        float ratio_to_max = state->frametimes[i] / state->graph_max;
        Rect bar_rect = {
            .pos = origin + Vec2f{i*bar_width},
            .size = Vec2f{bar_width, -ratio_to_max * graph_rect.size.y}
        };

        DrawFilledRect(bar_rect, c::red, true);
    }

    DrawUiText(c::frametime_max_text, RectTopRight(graph_rect) - Vec2f{2.f,2.f}, "%.2fms", state->graph_max);

    Vec2f pen = RectTopLeft(graph_rect) + Vec2f{2.f,2.f}; // with padding
    {
        auto response = DrawButton(c::frametime_scale_button, {pen, c::frametime_scale_button_size}, "+");
        if(response.pressed) state->graph_max /= c::frametime_scale_amount;
        pen.x += response.rect.size.x + 1.f; // 1.f padding
    }
    {
        auto response = DrawButton(c::frametime_scale_button, {pen, c::frametime_scale_button_size}, "-");
        if(response.pressed) state->graph_max *= c::frametime_scale_amount;
        pen.x += response.rect.size.x + 1.f;
    }
    {
        auto response = DrawButton(c::frametime_scale_button, {pen, c::frametime_scale_button_size}, "60fps");
        if(response.pressed) state->graph_max = 1000.f * (1.f/60.f);
        pen.x += response.rect.size.x + 1.f;
    }
    {
        auto response = DrawButton(c::frametime_scale_button, {pen, c::frametime_scale_button_size}, "1ms");
        if(response.pressed) state->graph_max = 1.f;
        pen.x += response.rect.size.x + 1.f;
    }
}