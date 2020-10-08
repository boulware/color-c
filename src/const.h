#ifndef CONST_H
#define CONST_H

#include "color.h"
#include "vec.h"
#include "text_render.h"
#include "imgui.h"
#include "table.h"

struct Ability;
struct Breed;
struct Unit;

namespace c
{
	// Colors
	const Color red 	= {1.f,0.f,0.f,1.f};
	const Color green 	= {0.f,1.f,0.f,1.f};
	const Color blue 	= {0.f,0.f,1.f,1.f};
	const Color lt_blue = {0.25f,0.25f,1.f,1.f};
	const Color black 	= {0.f,0.f,0.f,1.f};
	const Color white 	= {1.f,1.f,1.f,1.f};

	const Color dk_grey = {0.1f,0.1f,0.1f,1.f};
	const Color grey 	= {0.3f,0.3f,0.3f,1.f};
	const Color lt_grey = {0.5f,0.5f,0.5f,1.f};

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
	const Align align_rightcenter = {AlignX::right, AlignY::center};
	const Align align_bottomcenter = {AlignX::center, AlignY::bottom};

	const int max_formatted_string_length = 1024;

	const bool fullscreen = true;
	const int windowed_window_width = 1280;
	const int windowed_window_height = 720;
	const int fullscreen_window_width = 1600;
	const int fullscreen_window_height = 900;
	const float frametime_s = 1.f/60.f;

	const bool verbose_success_logging = false;
	const bool verbose_error_logging = false;

	int const c0_control_first = 0x0;
	int const c0_control_last = 0x1F;
	int const c1_control_first = 0x7F; // It's technically 0x80, but DEL is an "unofficial" control character.
	int const c1_control_last = 0x9F;

	u32 const control_code_backspace = 0x8;
	u32 const control_code_tab = 0x9;
	u32 const control_code_return = 0xD;
	u32 const control_code_DEL = 0x7F;

	const int printable_ascii_start = int(' ');
	const int printable_ascii_end = int('~')+1;
	const int printable_ascii_count = printable_ascii_end-printable_ascii_start;

	const int ability_table_partition_size = 20; // partition size in number of entities (not bytes)
	const int breed_table_partition_size = 20;
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
	const int max_effects_text_length = 1024;

	// Tables
	Id<Ability> null_ability_id = {0,0};
	Id<Breed> null_breed_id = {0,0};
	Id<Unit> null_unit_id = {0,0};

	// Debug
	const float error_flash_speed = 0.017f;

	// Drawing depths
	const float near_plane = 0.0f;
	const float far_plane = 1000.0f;
	const float clip_A = 2.0f/(far_plane-near_plane); // [A] in z'=Az+B (the depth=>clip space eqn)
	const float clip_B = -(far_plane+near_plane)/(far_plane-near_plane); // [B] in z'=Az+B

	const float hover_ui_draw_depth = 80.f;
	const float hud_draw_depth = 100.0f;
	const float field_draw_depth = 150.0f;

	// UI
	const TextLayout def_text_layout = {
		.font = &text_render::default_font,
		.color = c::white,
		.font_size = 32,
		.align = c::align_topleft,
		.draw_debug = false
	};

	const TextLayout error_text_layout = {
		.font = &text_render::default_font,
		.color = c::red,
		.font_size = 12,
		.align = c::align_topleft,
		.draw_debug = true
	};

	const ButtonLayout def_button_layout = {
		.label_layout = {
			.font = &text_render::default_font,
			.color = c::grey,
			.font_size = 32,
			.align = c::align_center,
			.draw_debug = false
		},
		.button_color = c::grey,
		.button_hover_color = c::white,
		.label_hover_color = c::white,
		.align = c::align_topleft
	};

	const ButtonLayout small_button_layout = {
		.label_layout = {
			.font = &text_render::default_font,
			.color = c::grey,
			.font_size = 16,
			.align = c::align_center,
			.draw_debug = false
		},
		.button_color = c::grey,
		.button_hover_color = c::white,
		.label_hover_color = c::white,
		.align = c::align_topleft
	};

	const TextLayout small_text_layout = {
		.font = &text_render::default_font,
		.color = c::white,
		.font_size = 16,
		.align = c::align_topleft,
		.draw_debug = false
	};

	const Vec2f def_text_entry_size = {300.f, 50.f};
	TextEntryLayout const def_text_entry_layout = {
		.size = c::def_text_entry_size,
		.label_layout = {
			.font = &text_render::default_font,
			.color = c::white,
			.font_size = 16,
			.align = c::align_topleft,
			.draw_debug = false
		},
		.text_layout = {
			.font = &text_render::default_font,
			.color = c::white,
			.font_size = 32,
			.align = c::align_leftcenter,
			.draw_debug = false,
			.max_width = c::def_text_entry_size.x
		},
		.align = c::align_topleft
	};

	ImguiContainer const def_ui_container = {
		.button_layout = c::def_button_layout,
		.text_entry_layout = c::def_text_entry_layout
	};

	// Main menu
	const TextLayout main_menu_unselected_text_layout = {
		.font = &text_render::default_font,
		.color = c::lt_grey,
		.font_size = 64,
		.align = c::align_topcenter,
		.draw_debug = false
	};

	const TextLayout main_menu_selected_text_layout = {
		.font = &text_render::default_font,
		.color = c::gold,
		.font_size = 64,
		.align = c::align_topcenter,
		.draw_debug = false
	};

	// Options menu
	const TextLayout options_selected_text_layout = {
		.font = &text_render::default_font,
		.color = c::gold,
		.font_size = 16,
		.align = c::align_topleft,
		.draw_debug = false
	};

	const TextLayout options_unselected_text_layout = {
		.font = &text_render::default_font,
		.color = c::white,
		.font_size = 16,
		.align = c::align_topleft,
		.draw_debug = false
	};

	const TextLayout options_modifying_text_layout = {
		.font = &text_render::default_font,
		.color = c::red,
		.font_size = 16,
		.align = c::align_topleft,
		.draw_debug = false
	};

	// Campaign


	// Force-directed graph parameters

	// Pretty good parameters for GenerateTreeFromMainBranch()
	const Vec2f node_size = {20.f, 20.f};
	const float fdg_edge_length          = 50.f;
	const float fdg_spring_constant      = 0.01f;
	const float fdg_charge_strength      = 1000.0f;
	const float fdg_friction             = 0.99f;

	const float max_speed = 2.f;

	// const Vec2f node_size = {40.f, 40.f};
	// const float fdg_edge_length          = 100.f;
	// const float fdg_spring_constant      = 0.01f;
	// const float fdg_charge_strength      = 1000.0f;
	// const float fdg_friction             = 0.9995f;


	// Editor
	const int max_field_text_length = 30;
	const int max_field_label_length = 30;
	int const editor_max_input_elements = 10;

	TextLayout editor_integer_box_label_layout = {
		.font = &text_render::default_font,
		.color = c::white,
		.font_size = 16,
		.align = c::align_bottomleft
	};

	TextLayout editor_integer_box_text_layout = {
		.font = &text_render::default_font,
		.color = c::white,
		.font_size = 32,
		.align = c::align_center
	};

	IntegerBoxLayout editor_vigor_integer_box_layout = {
		.size = {50.f,50.f},
		.border_color = c::red,
		.label_layout = c::editor_integer_box_label_layout,
		.text_layout = c::editor_integer_box_text_layout
	};

	IntegerBoxLayout editor_focus_integer_box_layout = {
		.size = {50.f,50.f},
		.border_color = c::yellow,
		.label_layout = c::editor_integer_box_label_layout,
		.text_layout = c::editor_integer_box_text_layout
	};

	IntegerBoxLayout editor_armor_integer_box_layout = {
		.size = {50.f,50.f},
		.border_color = c::lt_blue,
		.label_layout = c::editor_integer_box_label_layout,
		.text_layout = c::editor_integer_box_text_layout
	};

	// const ButtonLayout editor_list_button_layout = {
	// 	.label_layout = {
	// 		.font = &text_render::default_font,
	// 		.color = c::grey,
	// 		.font_size = 32,
	// 		.align = c::align_center,
	// 		.draw_debug = false
	// 	},
	// 	.button_color = c::grey,
	// 	.button_hover_color = c::white,
	// 	.label_hover_color = c::white,
	// 	.align = c::align_topleft
	// };

	// Unit frame
	const Vec2f unit_slot_size = {125.f, 400.f};
	const float unit_slot_padding = 50.f;
	const Vec2f unit_slot_name_offset = {0.5f*unit_slot_size.x, 0.f};
	const int unit_slot_name_size = 32;

	const Vec2f trait_bar_size = {125.f, 20.f};
	const float overheal_text_h_offset = 5.f;

	const Vec2f action_points_text_offset = {0.5f*unit_slot_size.x, 100.f};
	const TextLayout action_points_text_layout = {
		.font = &text_render::default_font,
		.color = c::grey,
		.font_size = 16,
		.align = c::align_topcenter,
		.draw_debug = false
	};

    TextLayout unit_frame_name_layout = {
    	.font = &text_render::default_font,
    	.color = c::white,
    	.font_size = 32,
    	.align = c::align_topcenter
    };

	// Unit info HUD
	const float hud_offset_from_bottom = 250.f;
	const Vec2f hud_unit_name_offset = {20.f,10.f};
	const Vec2f hud_ability_button_size = {300.f, 50.f};
	const Vec2f hud_ability_buttons_offset = {350.f, 10.f};
	const float hud_ability_button_padding = 5.f;

	const Vec2f ability_info_box_size = {500.f,225.f};
	const Color ability_info_bg_color = {0.1f, 0.1f, 0.1f, 1.f};
	const Vec2f hud_ability_info_offset = {700.f, 10.f};

	// Ability card
	const float tier_data_y_half_padding = 10.f;


	// const ButtonLayout ability_button_layout =
	// 	{{&text_render::default_font, c::white, 32, c::align_center, false},
	// 	c::white, c::white, c::white, c::align_topleft};

	const ButtonLayout unselected_ability_button_layout = {
		.label_layout = {
			.font = &text_render::default_font,
			.color = c::white,
			.font_size = 32,
			.align = c::align_center,
			.draw_debug = false
		},
		.button_color = c::white,
		.button_hover_color = c::yellow,
		.label_hover_color = c::yellow,
		.align = c::align_topleft
	};

	const ButtonLayout selected_ability_button_layout = {
		.label_layout = {
			.font = &text_render::default_font,
			.color = c::orange,
			.font_size = 32,
			.align = c::align_center,
			.draw_debug = false
		},
		.button_color = c::orange,
		.button_hover_color = c::orange,
		.label_hover_color = c::orange,
		.align = c::align_topleft
	};


	const TextLayout trait_bar_value_text_layout = {
		.font = &text_render::default_font,
		.color = c::white,
		.font_size = 16,
		.align = c::align_center,
		.draw_debug = false
	};

	const TextLayout trait_change_preview_text_layout = {
		.font = &text_render::default_font,
		.color = c::white,
		.font_size = 16,
		.align = c::align_leftcenter,
		.draw_debug = false
	};


	const float trait_change_preview_h_offset = 5.f;

	// End turn button
	const Vec2f end_turn_button_offset = {20.f, 100.f};
	const Vec2f end_turn_button_size = {150.f, 50.f};
	const Rect end_turn_button_rect = {end_turn_button_offset, end_turn_button_size};
	const ButtonLayout end_button_normal_layout = {
		.label_layout = {
			.font = &text_render::default_font,
			.color = c::white,
			.font_size = 32,
			.align = c::align_center,
			.draw_debug = false
		},
		.button_color = c::white,
		.button_hover_color = c::green,
		.label_hover_color = c::green,
		.align = c::align_topleft};

	const float end_button_clicked_time_s = 0.1f; // How long the button flashes after being clicked before greying out.
	const ButtonLayout end_button_clicked_layout = {
		.label_layout = {
			.font = &text_render::default_font,
			.color = c::red,
			.font_size = 32,
			.align = c::align_center,
			.draw_debug = false
		},
		.button_color = c::red,
		.button_hover_color = c::red,
		.label_hover_color = c::red,
		.align = c::align_topleft};

	const ButtonLayout end_button_disabled_layout = {
		.label_layout = {
			.font = &text_render::default_font,
			.color = c::dk_grey,
			.font_size = 32,
			.align = c::align_center,
			.draw_debug = false
		},
		.button_color = c::dk_grey,
		.button_hover_color = c::dk_grey,
		.label_hover_color = c::dk_grey,
		.align = c::align_topleft
	};

	// Enemy intents
	const Vec2f enemy_intent_offset = {0.5f*unit_slot_size.x, -50.f};
	const Vec2f enemy_intent_button_size = {150.f, 40.f};
	const ButtonLayout enemy_cannot_use_button_layout = {
		.label_layout = {
			.font = &text_render::default_font,
			.color = c::grey,
			.font_size = 24,
			.align = c::align_center,
			.draw_debug = false
		},
		.button_color = c::grey,
		.button_hover_color = c::lt_grey,
		.label_hover_color = c::lt_grey,
		.align = c::align_bottomcenter
	};
	const ButtonLayout enemy_can_use_button_layout = {
		.label_layout = {
			.font = &text_render::default_font,
			.color = c::white,
			.font_size = 24,
			.align = c::align_center,
			.draw_debug = false
		},
		.button_color = c::white,
		.button_hover_color = c::yellow,
		.label_hover_color = c::yellow,
		.align = c::align_bottomcenter
	};
	const ButtonLayout enemy_intented_button_layout = {
		.label_layout = {
			.font = &text_render::default_font,
			.color = c::orange,
			.font_size = 24,
			.align = c::align_center,
			.draw_debug = false
		},
		.button_color = c::orange,
		.button_hover_color = c::red,
		.label_hover_color = c::red,
		.align = c::align_bottomcenter
	};

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