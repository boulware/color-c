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
#include "pool.h"
#include "debug_overlay.h"
#include "frametimes_draw.h"

struct GameInitData
{
	PoolId<Arena> per_frame_arena_id;
	PoolId<Arena> permanent_arena_id;
	void *arena_pool_mutex_handle;
};

struct Game
{
    Pool<Arena> *arena_pool = nullptr;

	bool exit_requested;

	GameState current_state;
	bool state_entered;

	LogState log_state;

	float frame_time_ms;
	FrametimeGraphState *frametime_graph_state;
	bool draw_debug_overlay;
	DebugOverlay debug_overlay;

	ImguiContainer debug_container;

	InputState input;


	Texture temp_screen_texture;

	Framebuffer prepass_framebuffer;

	// Color shader
	GLuint color_shader;
	GLuint color_vao, color_vbo;

	// UV shader
	GLuint uv_shader;
	GLuint uv_vao, uv_vbo;

	// Computer shader visual effects
	GLuint blur_shader;
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