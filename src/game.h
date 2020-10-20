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
#include "camera.h"
#include "ai_explorer.h"

struct Game
{
	bool exit_requested;

	GameState current_state;
	bool state_entered;

	LogState log_state;
	float frame_time_ms;
	bool draw_debug_text;

	ImguiContainer debug_container;

	InputState input;

	GLuint prepass_fbo;
	GLuint prepass_texture;

	// Color shader
	GLuint color_shader;
	GLuint color_vao, color_vbo;

	// UV shader
	GLuint uv_shader;
	GLuint uv_vao, uv_vbo;

	// Gaussian blur
	GLuint blur_shader;
	GLuint blur_dst_texture;

	// Outline shader
	GLuint outline_shader;

	Vec2f window_size;

	Camera camera;

	GLuint temp_texture;
	Vec2i string_bmp_size;

	UnitSet player_party;
	Battle current_battle;

	Editor editor_state;
	OptionsMenu options_menu;
	MainMenu mainmenu_state;
	Campaign campaign;
	AiExplorer ai_explorer;
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