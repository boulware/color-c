#include "table_draw.h"

#include "table.h"

void InitTableDrawState(TableDrawState *state)
{
    state->cur_mode = TableDrawMode::Ability;
    state->ui_container = c::def_ui_container;
    state->ui_container.button_layout.label_layout.font_size = 16.f;
}

template<typename Type>
void
DrawTable(TableDrawState *state, Table<Type> *table, Rect window_rect)
{
    SetActiveContainer(&state->ui_container);
    ResetImguiContainer(&state->ui_container);
    state->ui_container.pos = window_rect.pos;
    state->ui_container.max_size = window_rect.size;

    {
        ButtonResponse response;

        // Ability
        if(state->cur_mode == TableDrawMode::Ability)
            state->ui_container.button_layout = c::active_debug_button;
        else
            state->ui_container.button_layout = c::inactive_debug_button;
        response = Button("Ability");
        if(response.pressed and TakeMouseFocus()) state->cur_mode = TableDrawMode::Ability;

        // Breed
        if(state->cur_mode == TableDrawMode::Breed)
            state->ui_container.button_layout = c::active_debug_button;
        else
            state->ui_container.button_layout = c::inactive_debug_button;
        response = Button("Breed");
        if(response.pressed and TakeMouseFocus()) state->cur_mode = TableDrawMode::Breed;

        // Unit
        if(state->cur_mode == TableDrawMode::Unit)
            state->ui_container.button_layout = c::active_debug_button;
        else
            state->ui_container.button_layout = c::inactive_debug_button;
        response = Button("Unit");
        if(response.pressed and TakeMouseFocus()) state->cur_mode = TableDrawMode::Unit;
    }

    Vec2f origin = Vec2f{window_rect.pos.x, BottomOfUiContainer(state->ui_container)};
    float y_padding_at_top = origin.y - window_rect.pos.y;

    int entries_per_dimension = (int)m::Ceil(m::Sqrt(table->max_entry_count));
    float entry_cell_size = (window_rect.size.y - y_padding_at_top) / entries_per_dimension;

    for(int i=0; i<table->max_entry_count; ++i)
    {
        int x = i % entries_per_dimension;
        int y = i / entries_per_dimension;

        auto entry = table->entries[i];

        Rect entry_rect = {
            .pos = origin + entry_cell_size*Vec2f{(float)x,(float)y},
            .size = entry_cell_size * Vec2f{1.f,1.f}
        };

        Color cell_color = c::black;
        if(entry.active) cell_color = c::green;

        DrawFilledRect(entry_rect, cell_color, true);
        DrawUnfilledRect(entry_rect, c::white, true);
    }
}

