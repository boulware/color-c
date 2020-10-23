#include "table_draw.h"

#include "table.h"

template<typename Type>
void
DrawTable(Table<Type> table, Vec2f origin, float square_size)
{
    int entries_per_dimension = (int)m::Ceil(m::Sqrt(table.max_entry_count));
    float entry_cell_size = square_size / entries_per_dimension;

    for(int i=0; i<table.max_entry_count; ++i)
    {
        int x = i % entries_per_dimension;
        int y = i / entries_per_dimension;

        auto entry = table.entries[i];

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