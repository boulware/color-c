#include "debug.h"

Rect
GetTimedBlockDataRect(Vec2f start_pos)
{
	Vec2f pos = start_pos;
	TextLayout function_cycles_layout = c::def_text_layout;
	function_cycles_layout.font_size = 16;

	u64 max_average_cycles = 0;
	float max_right_extent = 0.f;
	for(int i=0; i<debug::timed_block_array_size; i++)
	{
		if(TIMED_BLOCK_ARRAY[i].hit_count > 0)
		{
			u64 avg_cycles = TIMED_BLOCK_ARRAY[i].total_cycle_count/TIMED_BLOCK_ARRAY[i].hit_count;
			max_average_cycles = m::Max(avg_cycles, max_average_cycles);

			Vec2f text_size = SizeText(function_cycles_layout,
									   "%s count %llu, cycles %.3e, avg %llu",
									   TIMED_BLOCK_ARRAY[i].function_name,
									   TIMED_BLOCK_ARRAY[i].hit_count,
									   (double)TIMED_BLOCK_ARRAY[i].total_cycle_count,
									   avg_cycles);
			max_right_extent = m::Max(max_right_extent, start_pos.x + text_size.x);
			pos.y += text_size.y;
		}
	}

	float max_bar_width = 400.f;
	float bar_y = start_pos.y;
	for(int i=0; i<debug::timed_block_array_size; i++)
	{
		if(TIMED_BLOCK_ARRAY[i].hit_count > 0)
		{
			u64 avg_cycles = TIMED_BLOCK_ARRAY[i].total_cycle_count/TIMED_BLOCK_ARRAY[i].hit_count;
			float line_size = LineHeight(function_cycles_layout);
			// DrawFilledRect({{max_right_extent, bar_y},
			// 			   {max_bar_width*(float)avg_cycles/(float)max_average_cycles, line_size}},
			// 			   c::green,
			// 			   true);
			bar_y += line_size;
		}
	}

	Rect window_rect = {
		.pos = start_pos,
		.size = Vec2f{max_right_extent + max_bar_width - start_pos.x, pos.y - start_pos.y}
	};
	return window_rect;
}

Rect
DrawTimedBlockData(Vec2f start_pos)
{
	//SetDrawDepth(c::debug_overlay_draw_depth);

	Vec2f pos = start_pos;
	TextLayout function_cycles_layout = c::def_text_layout;
	function_cycles_layout.font_size = 16;

	u64 max_average_cycles = 0;
	float max_right_extent = 0.f;
	for(int i=0; i<debug::timed_block_array_size; i++)
	{
		if(TIMED_BLOCK_ARRAY[i].hit_count > 0)
		{
			u64 avg_cycles = TIMED_BLOCK_ARRAY[i].total_cycle_count/TIMED_BLOCK_ARRAY[i].hit_count;
			max_average_cycles = m::Max(avg_cycles, max_average_cycles);

			Vec2f text_size = DrawUiText(function_cycles_layout, pos,
									   "%s count %llu, cycles %.3e, avg %llu",
									   TIMED_BLOCK_ARRAY[i].function_name,
									   TIMED_BLOCK_ARRAY[i].hit_count,
									   (double)TIMED_BLOCK_ARRAY[i].total_cycle_count,
									   avg_cycles).size;
			max_right_extent = m::Max(max_right_extent, start_pos.x + text_size.x);
			pos.y += text_size.y;
		}
	}

	float max_bar_width = 400.f;
	float bar_y = start_pos.y;
	for(int i=0; i<debug::timed_block_array_size; i++)
	{
		if(TIMED_BLOCK_ARRAY[i].hit_count > 0)
		{
			u64 avg_cycles = TIMED_BLOCK_ARRAY[i].total_cycle_count/TIMED_BLOCK_ARRAY[i].hit_count;
			float line_size = LineHeight(function_cycles_layout);
			DrawFilledRect({{max_right_extent, bar_y},
						   {max_bar_width*(float)avg_cycles/(float)max_average_cycles, line_size}},
						   c::green,
						   true);
			bar_y += line_size;
		}
	}

	Rect window_rect = {
		.pos = pos,
		.size = Vec2f{max_right_extent + max_bar_width, pos.y - start_pos.y}
	};
	return window_rect;
}