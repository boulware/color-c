#include <intrin.h>
#include <ctime>

#include "macros.h"
#include "const.h"
#include "global.h"
#include "debug.h"

#include "platform.h"
#include "opengl.h"
#include "game.h"

#define MAX_SHADER_SOURCE_SIZE 10000
#define MAX_SHADER_LOG_SIZE 10000

Platform *platform = nullptr;
OpenGL *gl = nullptr;
Game *game = nullptr;

#include "log.cpp"
#include "util.cpp"
#include "opengl.cpp"
#include "math.cpp"
#include "bitmap.cpp"
#include "vec.cpp"
#include "text_render.cpp"
#include "freetype_wrapper.cpp"
#include "text_parsing.cpp"
#include "data_table.cpp"
#include "imgui.cpp"
#include "draw.cpp"
#include "input.cpp"
#include "image.cpp"
#include "types.cpp"
#include "sprite.cpp"
#include "align.cpp"
#include "memory.cpp"
#include "debug.cpp"
#include "battle.cpp"
#include "unit.cpp"
#include "passive_skill_tree.cpp"
#include "timer.cpp"
#include "oscillating_timer.cpp"
#include "random.cpp"
#include "traitset.cpp"
#include "ability.cpp"
#include "target_class.cpp"
#include "editor.cpp"
#include "string.cpp"
#include "utf32string.cpp"

#include "meta_print(manual).cpp"

extern "C" void
GameHook(Platform *platform_, OpenGL *gl_, Game *game_)
{
	platform = platform_;
	gl = gl_;
	game = game_;
}

extern "C" void
GameInit()
{
	memory::per_frame_arena = AllocateArena();
	memory::permanent_arena = AllocateArena();

	InitLcgSystemSeed(&random::default_lcg);

	game->temp_texture = GenerateAndBindTexture();
	gl->Enable(GL_BLEND);
	gl->BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	if(InitFreetype(&game->ft_lib))
	{
		FT_Face roboto_face = LoadFontFaceFromFile("resource/Roboto-Bold.ttf", game->ft_lib);
		text_render::default_font = LoadFontData(roboto_face, 64);
	}

	input::global_input = &game->input;

	game->color_shader = GenerateShaderProgramFromFiles("src/color_vertex.glsl", "src/color_fragment.glsl");
	gl->BindFragDataLocation(game->color_shader, 0, "frag_color");
	gl->ProgramUniform4f(game->color_shader, 1, 1.f, 1.f, 1.f, 1.f); // draw color; default white

	game->uv_shader = GenerateShaderProgramFromFiles("src/uv_vertex.glsl", "src/uv_fragment.glsl");
	gl->BindFragDataLocation(game->uv_shader, 0, "frag_color");
	gl->ProgramUniform1i(game->uv_shader, 1, 0); // sampling texture index; default 0 index

	gl->ProgramUniform2fv(game->color_shader, 0, 1, (GLfloat*)&game->window_size);
	gl->ProgramUniform2fv(game->uv_shader, 0, 1, (GLfloat*)&game->window_size);

	// Set up UV VAO/VBO
	gl->GenVertexArrays(1, &game->uv_vao);
	gl->BindVertexArray(game->uv_vao);
	gl->GenBuffers(1, &game->uv_vbo);
	gl->BindBuffer(GL_ARRAY_BUFFER, game->uv_vbo);
	gl->EnableVertexAttribArray(0);
	gl->EnableVertexAttribArray(1);
	gl->VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
							4*sizeof(GLfloat), 0);
	gl->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
	 						4*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));

	// Set up color VAO/VBO
	gl->GenVertexArrays(1, &game->color_vao);
	gl->BindVertexArray(game->color_vao);
	gl->GenBuffers(1, &game->color_vbo);
	gl->BindBuffer(GL_ARRAY_BUFFER, game->color_vbo);
	gl->EnableVertexAttribArray(0);
	gl->VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
							0, 0);

	gl->PixelStorei(GL_UNPACK_ALIGNMENT, 1);
	gl->Enable(GL_BLEND);

	// Ability table
	g::ability_table = AllocDataTable(sizeof(Ability), c::ability_table_partition_size);
	LoadAbilityFile("data/ability.dat", &g::ability_table);
	auto ability_table_copy = g::ability_table;

	// Unit schematic table
	g::unit_schematic_table = AllocDataTable(sizeof(UnitSchematic), c::unit_schematic_table_partition_size);
	LoadUnitSchematicFile("data/unit_schematic.dat", &g::unit_schematic_table, g::ability_table);
	auto schematic_table_copy = g::unit_schematic_table;

	// Unit table
	g::unit_table = AllocDataTable(sizeof(Unit), c::unit_table_partition_size);
	auto unit_table_copy = g::unit_table;

	// Passive skill table
	g::passive_skill_table = AllocDataTable(sizeof(PassiveSkill), c::passive_skill_table_partition_size);
	{
		PassiveSkill passives[] = {{"Equilibrium"}, {"Potency"}, {"Constitution"}, {"Bravery"}, {"Stoicism"}};

		for(PassiveSkill passive : passives)
		{
			*(PassiveSkill*)CreateEntry(&g::passive_skill_table) = passive;
		}
	}

	game->player_party[0] = CreateUnitByName("Rogue", Team::allies);
	game->player_party[1] = CreateUnitByName("Warrior", Team::allies);
	game->player_party[2] = CreateUnitByName("Archer", Team::allies);
	game->player_party[3] = CreateUnitByName("Cleric", Team::allies);

	game->current_battle = {};
	game->current_battle.hud = {{0.f, game->window_size.y-c::hud_offset_from_bottom}, {game->window_size.x, c::hud_offset_from_bottom}};

	// CreateUnit("Wolf", Team::enemies, &game->enemies[0]);
	// CreateUnit("Slime", Team::enemies, &game->enemies[1]);

	// Generate unit slots
	{
		Vec2f pos = {50.f, 200.f};

		for(Vec2f &slot : game->current_battle.unit_slots)
		{
			slot = pos;
			pos.x += c::unit_slot_size.x + c::unit_slot_padding;
		}
	}

	// Place units in unit slots
	for(int i=0; i<c::max_party_size; i++)
	{
		game->current_battle.units[i] = game->player_party[i];
	}
	game->current_battle.units[c::max_party_size+0] = CreateUnitByName("Dragon", Team::enemies);
	// game->current_battle.units[c::max_party_size+1] = CreateUnitByName("Slime", Team::enemies);
	// game->current_battle.units[c::max_party_size+2] = CreateUnitByName("Wolf", Team::enemies);
	// game->current_battle.units[c::max_party_size+3] = CreateUnitByName("Slime", Team::enemies);

	// imgui
	game->debug_container = c::def_ui_container;
	game->debug_container.max_size = {300.f,200.f};

	// Cursor
	game->pointer_cursor = LoadBitmapFileIntoSprite("resource/cursor.bmp");
	game->target_cursor = LoadBitmapFileIntoSprite("resource/target.bmp", c::align_center);
	game->red_target_cursor = LoadBitmapFileIntoSprite("resource/target_red.bmp", c::align_center);

	InitiateBattle(&game->current_battle);
	StartEditor(&game->editor_state);

	// Debug
	debug::start_count = __rdtsc();
	//debug::timed_block_array_size = __COUNTER__;
	debug::cycles_per_second = platform->PerformanceCounterFrequency();
	game->draw_debug_text = false;
	game->test_float = 0.f;

	game->current_state = GameState::Editor;

//
	#define OPTIMIZE false
	#if OPTIMIZE
	#define OPTIMIZING_FUNCTION ClearArena
	#define SLOW_FUNCTION PASTE(OPTIMIZING_FUNCTION, Slow)
	#define FAST_FUNCTION PASTE(OPTIMIZING_FUNCTION, Fast)
	OPTIMIZING_FUNCTION = PASTE(OPTIMIZING_FUNCTION, Slow);
	#endif
}

extern "C" void
GameUpdateAndRender()
{
	//TIMED_BLOCK;
	debug::timed_block_array_size = __COUNTER__;

	// Utf32String foo = {};
	// foo.length = 5;
	// foo.max_length = 100;
	// foo.data = (u32*)0x123456;
	// String foo_as_string = AsString(foo);
	// //EasyDrawText("%.*s", (int)foo_as_string.length, foo_as_string.data);
	// DrawTextMultiline(c::def_text_layout, {0.f,0.f}, "%.*s", (int)foo_as_string.length, foo_as_string.data);

	if(g::error_flash_increasing)
	{
		g::error_flash_counter += c::error_flash_speed;
		if(g::error_flash_counter > 1.f) g::error_flash_increasing = false;
	}
	else
	{
		g::error_flash_counter -= c::error_flash_speed;
		if(g::error_flash_counter < 0.f) g::error_flash_increasing = true;
	}

	if(Pressed(vk::F5))
	{
		#if OPTIMIZE
		if(OPTIMIZING_FUNCTION == SLOW_FUNCTION) OPTIMIZING_FUNCTION = FAST_FUNCTION;
		else OPTIMIZING_FUNCTION = SLOW_FUNCTION;
		#endif
	}

	ClearArena(&memory::per_frame_arena);

	// Reset timed block data
	if(Pressed(vk::R))
	{
		for(int i=0; i<debug::timed_block_array_size; i++)
		{
			TIMED_BLOCK_ARRAY[i].hit_count = 0;
			TIMED_BLOCK_ARRAY[i].total_cycle_count = 0;
		}
	}

	if(Pressed(vk::tilde)) game->draw_debug_text = !game->draw_debug_text;

	Vec2f pos = {1600.f,0.f};
	TextLayout frametime_layout = c::def_text_layout;
	frametime_layout.align = c::align_topright;
	pos.y += DrawText(frametime_layout, pos, "frame: %.3fms", game->frame_time_ms).y;

	// Right click cancels selected ability if one is selected.
	// If no ability is selected, cancels selected unit if one is selected.
	// If neither a unit nor ability is selected, do nothing
	if(Pressed(vk::rmb))
	{
		if(game->current_battle.selected_ability != nullptr)
		{
			game->current_battle.selected_ability = nullptr;
//			game->current_battle.selected_ability_valid_target_set = {};
		}
		else if(game->current_battle.selected_unit != nullptr)
		{
			game->current_battle.selected_unit = nullptr;
		}
	}

	// Reset pen position to container origin for all gui containers
	ResetImguiContainer(&game->debug_container);

	// Function keys to switch between game states.
	if(Pressed(vk::F1)) game->current_state = GameState::Battle;
	if(Pressed(vk::F2)) game->current_state = GameState::Editor;

	if(game->current_state == GameState::Battle)
	{
		UpdateBattle(&game->current_battle);
	}
	else if(game->current_state == GameState::Editor)
	{
		UpdateAndDrawEditor(&game->editor_state);
	}

	if(game->draw_debug_text)
	{
		DrawTimedBlockData();
	}

	//DrawText(c::def_text_layout, {500.f,500.f}, "%zu", ArenaBytesAllocated(memory::per_frame_arena));

	// Log timed block data and exit game.
	if(Pressed(vk::escape))
	{
		LogToFile("logs/DebugTimings.txt", "-----------------------");
		for(int i=0; i<debug::timed_block_array_size; i++)
		{
			TimedBlockEntry *entry = TIMED_BLOCK_ARRAY+i;
			if(entry->function_name == nullptr) continue;

			if(entry->hit_count > 0)
			{
				LogToFile("logs/DebugTimings.txt",
						  "%s : (hit %llu, cycles %llu, c/h %llu)",
						  entry->function_name,
						  entry->hit_count,
						  entry->total_cycle_count,
						  entry->total_cycle_count/entry->hit_count);
			}
			else
			{
				LogToFile("logs/DebugTimings.txt",
						  "%s : (hit %llu, cycles %llu, c/h ?)",
						  entry->function_name,
						  entry->hit_count,
						  entry->total_cycle_count);
			}

		}

		game->exit_requested = true;
	}
}

TimedBlockEntry TIMED_BLOCK_ARRAY[__COUNTER__-1];