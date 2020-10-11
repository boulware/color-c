#ifndef GAME_H
#define GAME_H

#include "const.h"
#include "log.h"
#include "vec.h"
#include "utf32string.h"
#include "freetype_wrapper.h"
#include "unit.h"

#include "input.h"
#include "imgui.h"
#include "sprite.h"
#include "battle.h"
#include "editor.h"
#include "main_menu.h"
#include "options_menu.h"
#include "game_state.h"
#include "campaign.h"
#include "test_mode.h"
#include "generate_node_graph_params.h"

struct Game
{
	bool exit_requested;

	GameState current_state;

	LogState log_state;
	float frame_time_ms;
	bool draw_debug_text;

	ImguiContainer debug_container;

	InputState input;

	GLuint color_shader, uv_shader;
	GLuint color_vao, color_vbo;
	GLuint uv_vao, uv_vbo;

	Vec2f window_size;

	Vec2f camera_pos;

	GLuint temp_texture;
	Vec2i string_bmp_size;

	UnitSet player_party;
	Battle current_battle;

	Editor editor_state;
	OptionsMenu options_menu;
	MainMenu mainmenu_state;
	Campaign campaign;
	TestMode test_mode;

	Sprite pointer_cursor;
	Sprite target_cursor;
	Sprite red_target_cursor;

	GenerateNodeGraph_Params generate_node_graph_params;
	int number_of_arenas_allocated;

	// test/debug
	float test_float;
	int test_int;
};

#endif