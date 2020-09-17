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

struct Game
{
	LogState log_state;

	ImguiContainer debug_container;
	ImguiContainer ally_container;
	ImguiContainer enemy_container;
	ImguiContainer hud_abilities_layout;

	InputState input;
	Vec2f mouse_pos;

	GLuint color_shader, uv_shader;
	GLuint color_vao, color_vbo;
	GLuint uv_vao, uv_vbo;

	Vec2f window_size;

	GLuint temp_texture;
	Vec2i string_bmp_size;

	FT_Library ft_lib;
	Font default_font;

	DataTable ability_table;
	DataTable unit_table;

	Unit allies[c::max_party_size];
	Unit enemies[c::max_party_size];

	TargetSet all_targets;
	TargetSet _selected_ability_valid_targets;
	// The targets that are inferred from the currently selected ability
	// and the currently hovered unit.
	// e.g., if you select an enemy Wolf while an ability that targets all enemies is selected,
	// the selected target is the Wolf, but the inferred target set is all enemies (which
	// is contextually inferred based on the targeting type of the currently selected ability)
	TargetSet _inferred_target_set;

	Sprite pointer_cursor;
	Sprite target_cursor;
	Sprite red_target_cursor;

	Unit *selected_unit;
	Ability *_selected_ability;

	float test_float;
	int test_int;
};

#endif