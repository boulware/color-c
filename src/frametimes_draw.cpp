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

    state->ui_container = c::def_ui_container;
    state->ui_container.button_layout.label_layout.font_size = 16.f;

    state->label_text = {
        .font = &text_render::default_font,
        .color = c::lt_grey,
        .hover_color = c::white,
        .has_hover_color = true,
        .font_size = 16,
        .align = c::align_topright,
    };
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
    if(state->entry_count <= 0) return;
    // DrawFilledRect(graph_rect, c::vdk_red, true);
    // DrawUnfilledRect(graph_rect, c::white, true);

    // float max_frametime = 0.f;
    // for(int i=0; i<frametime_count; ++i)
    // {
    //     max_frametime = m::Max(frametimes[i], max_frametime);
    // }

    float bar_width = graph_rect.size.x / state->entry_count;
    Vec2f origin = RectBottomLeft(graph_rect);
    float frametime_sum = 0.f;
    float frametime_min = state->frametimes[0];
    float frametime_max = state->frametimes[0];
    for(int i=0; i<state->entry_count; ++i)
    {
        frametime_sum += state->frametimes[i];
        frametime_min = m::Min(state->frametimes[i], frametime_min);
        frametime_max = m::Max(state->frametimes[i], frametime_max);
    }
    float frametime_avg = frametime_sum / state->entry_count;

    bool highlight_min = false;
    bool highlight_max = false;
    bool highlight_avg = false;

    // Draw tracking bar in background
    {
        float ratio_to_max = 1.f;
        Rect bar_rect = {
            .pos = origin + Vec2f{state->cur_frametime_index*bar_width, 0.f},
            .size = Vec2f{bar_width, -ratio_to_max * graph_rect.size.y}
        };

        Color bar_color = c::grey;
        DrawFilledRect(bar_rect, bar_color, true);
    }

    // Draw bars
    for(int i=0; i<state->entry_count; ++i)
    {
        float ratio_to_max = state->frametimes[i] / state->graph_max;
        float clamped_ratio = m::Min(ratio_to_max, 1.f);
        Rect bar_rect = {
            .pos = origin + Vec2f{i*bar_width, 0.f},
            .size = Vec2f{bar_width, -clamped_ratio * graph_rect.size.y}
        };

        Color bar_color = c::green;
        if(highlight_min and state->frametimes[i] == frametime_min)
            bar_color = c::white;
        else if(highlight_max and state->frametimes[i] == frametime_max)
            bar_color = c::white;
        else if(ratio_to_max > 1.f)
            bar_color = c::red;

        DrawFilledRect(bar_rect, bar_color, true);
    }

    Vec2f pen = RectTopRight(graph_rect) - Vec2f{2.f,2.f};
    pen.y += DrawText(c::frametime_max_text, pen, "scale: %.2fms", state->graph_max).rect.size.y;
    {
        auto response = DrawText(state->label_text, pen, "min: %.2fms", frametime_min);
        if(response.hovered) highlight_min = true;
        pen.y += response.rect.size.y;
    }
    {
        auto response = DrawText(state->label_text, pen, "max: %.2fms", frametime_max);
        if(response.hovered) highlight_max = true;
        pen.y += response.rect.size.y;
    }
    {
        auto response = DrawText(state->label_text, pen, "avg: %.2fms", frametime_avg);
        if(response.hovered) highlight_avg = true;
        pen.y += response.rect.size.y;
    }

    // Highlight min/max frametime
    for(int i=0; i<state->entry_count; ++i)
    {
        Color bar_color = c::green;
        if(highlight_min and state->frametimes[i] == frametime_min)
            bar_color = c::white;
        else if(highlight_max and state->frametimes[i] == frametime_max)
            bar_color = c::white;
        else continue;

        float ratio_to_max = state->frametimes[i] / state->graph_max;
        float clamped_ratio = m::Min(ratio_to_max, 1.f);
        Rect bar_rect = {
            .pos = origin + Vec2f{i*bar_width, 0.f},
            .size = Vec2f{bar_width, -clamped_ratio * graph_rect.size.y}
        };

        DrawFilledRect(bar_rect, bar_color, true);
    }
    // Draw bars
    //SetDrawDepth(c::debug_overlay_draw_depth);

    if(highlight_avg)
    {
        float ratio_to_max = frametime_avg / state->graph_max;
        float clamped_ratio = m::Min(ratio_to_max, 1.f);
        float avg_y = origin.y - (clamped_ratio * graph_rect.size.y);

        Vec2f start_pt = Vec2f{RectBottomLeft(graph_rect).x, avg_y};
        Vec2f end_pt   = Vec2f{RectBottomRight(graph_rect).x, avg_y};
        DrawLine(start_pt, end_pt, c::white);
    }

    // DrawTextMultiline(c::frametime_max_text, RectTopRight(graph_rect) - Vec2f{2.f,2.f},
    //                   "scale: %.2fms\nmin: %.2fms\nmax: %.2fms\navg: %.2fms",
    //                   state->graph_max,
    //                   frametime_min,
    //                   frametime_max,
    //                   frametime_avg);


    { // Buttons
        ResetImguiContainer(&state->ui_container);
        SetActiveContainer(&state->ui_container);
        state->ui_container.pos = graph_rect.pos;
        state->ui_container.max_size = graph_rect.size;

        ButtonResponse response;

        response = Button("+");
        if(response.pressed and TakeMouseFocus()) state->graph_max /= c::frametime_scale_amount;

        response = Button("-");
        if(response.pressed and TakeMouseFocus()) state->graph_max *= c::frametime_scale_amount;

        response = Button("60fps");
        if(response.pressed and TakeMouseFocus()) state->graph_max = 1000.f * (1.f/60.f);

        response = Button("1ms");
        if(response.pressed and TakeMouseFocus()) state->graph_max = 1.f;
    }
/*
    Vec2f pen = RectTopLeft(graph_rect) + Vec2f{2.f,2.f}; // with padding
    {
        auto response = DrawButton(c::frametime_scale_button, {pen, c::frametime_scale_button_size}, "+");
        if(response.pressed)
        {
            state->graph_max /= c::frametime_scale_amount;

        }
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
    */
}