#ifndef GAME_H
#define GAME_H

#include "const.h"
#include "log.h"
#include "vec.h"
#include "string.h"
#include "food.h"
#include "freetype_wrapper.h"
#include "unit.h"
#include "data_table.h"
#include "input.h"
#include "imgui.h"
#include "sprite.h"
#include "battle.h"

struct Game
{
	bool exit_requested;

	LogState log_state;
	float frame_time_ms;
	bool draw_debug_text;

	ImguiContainer debug_container;

	InputState input;

	GLuint color_shader, uv_shader;
	GLuint color_vao, color_vbo;
	GLuint uv_vao, uv_vbo;

	Vec2f window_size;

	GLuint temp_texture;
	Vec2i string_bmp_size;

	FT_Library ft_lib;

	Unit *player_party[c::max_party_size];
	Battle current_battle;

	Sprite pointer_cursor;
	Sprite target_cursor;
	Sprite red_target_cursor;

	// test/debug
	float test_float;
	int test_int;
};

#endif