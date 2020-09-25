#ifndef CONST_H
#define CONST_H

#include "color.h"
#include "vec.h"
#include "text_render.h"
#include "imgui.h"
#include "data_table.h"

namespace c
{
	// Colors
	const Color red 	= {1.f,0.f,0.f,1.f};
	const Color green 	= {0.f,1.f,0.f,1.f};
	const Color blue 	= {0.f,0.f,1.f,1.f};
	const Color lt_blue = {0.25f,0.25f,1.f,1.f};
	const Color black 	= {0.f,0.f,0.f,1.f};
	const Color white 	= {1.f,1.f,1.f,1.f};
	const Color grey 	= {0.5f,0.5f,0.5f,1.f};
	const Color lt_grey = {0.7f,0.7f,0.7f,1.f};
	const Color dk_grey = {0.2f,0.2f,0.2f,1.f};
	const Color yellow	= {1.f,1.f,0.f,1.f};
	const Color gold 	= {0.8f,0.8f,0.f,1.f};
	const Color orange	= {1.f,0.4f,0.f,1.f};

	// Alignments
	const Align align_topleft = {AlignX::left, AlignY::top};
	const Align align_topright = {AlignX::right, AlignY::top};
	const Align align_center = {AlignX::center, AlignY::center};
	const Align align_bottomleft = {AlignX::left, AlignY::bottom};
	const Align align_topcenter = {AlignX::center, AlignY::top};
	const Align align_leftcenter = {AlignX::left, AlignY::center};
	const Align align_bottomcenter = {AlignX::center, AlignY::bottom};

	const int max_formatted_string_length = 1024;

	const bool fullscreen = true;
	const int windowed_window_width = 1280;
	const int windowed_window_height = 720;
	const int fullscreen_window_width = 1600;
	const int fullscreen_window_height = 900;
	const float frametime_s = 1.f/60.f;

	const bool verbose_success_logging = false;
	const bool verbose_error_logging = true;

	const int printable_ascii_start = int(' ');
	const int printable_ascii_end = int('~')+1;
	const int printable_ascii_count = printable_ascii_end-printable_ascii_start;

	const int ability_table_partition_size = 20; // partition size in number of entities (not bytes)
	const int unit_schematic_table_partition_size = 20;
	const int unit_table_partition_size = 20;
	const int passive_skill_table_partition_size = 20;
	const int passive_node_table_partition_size = 20;

	const char whitespace[] = " \t\n\v\f\r";
	const int max_s32_digits = 11; // including '-' or '+'
	const u32 medium_text = 24;

	const int scratch_string_size = 1024;

	const int max_unit_name_length = 20;
	const int max_ability_name_length = 20;
	const int max_passive_skill_name_length = 20;

	// Debug
	const float error_flash_speed = 0.017f;

	// UI
	const TextLayout def_text_layout = {&text_render::default_font, c::white, 32, c::align_topleft, false};
	const TextLayout error_text_layout = {&text_render::default_font, c::red, 12, c::align_topleft, true};
	const ButtonLayout def_button_layout = {{&text_render::default_font, c::grey, 32, c::align_center, false},
											c::grey, c::white, c::white, c::align_topleft};


	// Unit frame
	const Vec2f unit_slot_size = {125.f, 400.f};
	const float unit_slot_padding = 50.f;
	const Vec2f unit_slot_name_offset = {0.5f*unit_slot_size.x, 0.f};
	const int unit_slot_name_size = 32;

	const Vec2f trait_bar_size = {125.f, 20.f};
	const float overheal_text_h_offset = 5.f;

	const Vec2f action_points_text_offset = {0.5f*unit_slot_size.x, 100.f};
	const TextLayout action_points_text_layout = {&text_render::default_font, c::grey, 16, c::align_topcenter, false};

	const Vec2f ability_info_box_size = {300.f,150.f};
	const Color ability_info_bg_color = {0.1f,0.1f,0.1f,1.f};

	// Unit info HUD
	const float hud_offset_from_bottom = 250.f;
	const Vec2f hud_ability_button_size = {300.f, 50.f};
	const Vec2f hud_ability_buttons_offset = {350.f, 10.f};
	const Vec2f hud_ability_info_offset = {700.f, 20.f};
	const float hud_ability_button_padding = 5.f;

	const ButtonLayout ability_button_layout =
		{{&text_render::default_font, c::white, 32, c::align_center, false},
		c::white, c::white, c::white, c::align_topleft};

	const ButtonLayout selected_ability_button_layout =
		{{&text_render::default_font, c::orange, 32, c::align_center, false},
		c::orange, c::orange, c::orange, c::align_topleft};

	const ButtonLayout hovered_ability_button_layout =
		{{&text_render::default_font, c::yellow, 32, c::align_center, false},
		c::yellow, c::yellow, c::yellow, c::align_topleft};

	const TextLayout trait_bar_value_text_layout = {&text_render::default_font, c::white, 16, c::align_center, false};

	const TextLayout trait_change_preview_text_layout = {&text_render::default_font, c::white, 16, c::align_leftcenter, false};
	const float trait_change_preview_h_offset = 5.f;

	// End turn button
	const Vec2f end_turn_button_offset = {20.f, 100.f};
	const Vec2f end_turn_button_size = {150.f, 50.f};
	const Rect end_turn_button_rect = {end_turn_button_offset, end_turn_button_size};
	const ButtonLayout end_button_normal_layout =
		{{&text_render::default_font, c::white, 32, c::align_center, false},
		c::white, c::green, c::green, c::align_topleft};

	const float end_button_clicked_time_s = 0.1f; // How long the button flashes after being clicked before greying out.
	const ButtonLayout end_button_clicked_layout =
		{{&text_render::default_font, c::red, 32, c::align_center, false},
		c::red, c::red, c::red, c::align_topleft};

	const ButtonLayout end_button_disabled_layout =
		{{&text_render::default_font, c::dk_grey, 32, c::align_center, false},
		c::dk_grey, c::dk_grey, c::dk_grey, c::align_topleft};

	// Enemy intents
	const Vec2f enemy_intent_offset = {0.5f*c::unit_slot_size.x, -50.f};
	const Vec2f enemy_intent_button_size = {150.f, 40.f};
	const ButtonLayout enemy_intent_button_layout =
		{{&text_render::default_font, c::grey, 16, c::align_center, false},
		c::grey, c::lt_grey, c::lt_grey, c::align_center};

	// traitset string format: "%dV %dF %dA"
	// This requires space for up to:
	// 		3 s32 (3*11=33)
	// 		3 trait identifier characters (3)
	//		2 spaces (2)
	// 		1 null terminator (1)
	//		TOTAL = 33+3+2+1 = 39
	const int max_traitset_string_size = 3*c::max_s32_digits + 3 + 2 + 1;

	// Gameplay
	const int moveset_max_size = 4;
	const int max_party_size = 4;
	const int max_target_count = 2*max_party_size;

	const int max_tree_node_children = 2;
	const int trait_count = 3;

	const int max_ability_tier_count = 3;
	const int max_effect_count = 10;
};

#endif