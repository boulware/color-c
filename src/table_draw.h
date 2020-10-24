#ifndef TABLE_DRAW_H
#define TABLE_DRAW_H

enum class TableDrawMode
{
    Ability,
    Breed,
    Unit,
    COUNT
};

struct
TableDrawState
{
    TableDrawMode cur_mode;
    ImguiContainer ui_container;
};

template<typename Type> void DrawTable(TableDrawState *state, Table<Type> *table, Rect window_rect);
void InitTableDrawState(TableDrawState *state);

#endif